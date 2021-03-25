/*
	TinyPixel library
	R.J.Tidey Mar 22 2021
	Based on timing ideas and SimplePixelDemo
	at http://wp.josh.com/2014/05/11/ws2812-neopixels-made-easy/

	Can be used unbuffered to allow very long pixel strings with values constructed on the fly
	or buffered to allow simple updating of individual pixels
*/
#include "TinyPixel.h"

// nominal pulse width periods in nS
#define T1H  900    // High pulse for a 1
#define T1L  350    // Low pulse for a 1
#define T0H  350    // Width of a 0 bit in ns
#define T0L  500    // Width of a 0 bit in ns

#define LATCH_USEC 260    // Width of the low gap between bits to cause a frame to latch

// Macros to calculate delay in clock periods
#define NS_PER_CLOCK ( 10000 / (F_CPU / 100000) )
#define DELAY_OPS1(d) ( d /   NS_PER_CLOCK )
#define DELAY_OPS(d, s) ( (d - s * NS_PER_CLOCK) / NS_PER_CLOCK )

volatile uint8_t tp_ledPinMask;
volatile uint16_t tp_pixelCount;
GRBPixel* tp_buffer = NULL;

// Dynamically transmit a GRBPixel out
void tp_sendGRBPixel(GRBPixel* pixelPtr) {
	asm volatile (
		"in r18, 0x3f \n\t" //save status reg
		"lds r19, (tp_ledPinMask) \n\t"
		"mov __tmp_reg__, r19 \n\t"
		"ldi r20, 3 \n\t"
		"1: ; byte loop start \n\t"
		"ld r21, X \n\t"
		"ldi r19, 8 \n\t"
		"2: ; bit loop start \n\t"
		"lsl r21 \n\t"
		"brcc 3f \n\t"
		"out %[toggle], __tmp_reg__ \n\t"	// Toggle output bit high
		".rept %[onCycles1] \n\t"		// Execute NOPs to delay exactly the specified number of cycles
		"nop \n\t"
		".endr \n\t"
		"out %[toggle], __tmp_reg__ \n\t"	// Toggle output bit low
		".rept %[offCycles1] \n\t"		// Execute NOPs to delay exactly the specified number of cycles
		"nop \n\t"
		".endr \n\t"
		"rjmp 4f \n\t"
		"3: \n\t"
		"cli \n\t"						// No interrupts during critical 0 high pulse
		"out %[toggle], __tmp_reg__ \n\t"	// Toggle output bit high
		".rept %[onCycles0] \n\t"		// Now timing actually matters. The 0-bit must be long enough to be detected but not too long or it will be a 1-bit
		"nop \n\t"						// Execute NOPs to delay exactly the specified number of cycles
		".endr \n\t"
		"out %[toggle], __tmp_reg__ \n\t"	// Toggle output bit low
		"out 0x3f, r18 \n\t"			// restore status reg
		".rept %[offCycles0] \n\t"		// Execute NOPs to delay exactly the specified number of cycles
		"nop \n\t"
		".endr \n\t"
		"4: \n\t"
		"dec r19 \n\t"
		"brne 2b \n\t"
		"adiw X, 1 \n\t"
		"dec r20 \n\t"
		"brne 1b \n\t"
		::
		"x" (pixelPtr),
		[toggle]	 "I" (_SFR_IO_ADDR(PIXEL_TOGGLE)),
		[onCycles1]	 "I" (DELAY_OPS( T1H, 2 ) ),	// 1-bit width less overhead  for the actual bit setting
		[offCycles1] "I" (DELAY_OPS( T1L, 5 ) ),	// Minimum interbit delay.
		[onCycles0]	 "I" (DELAY_OPS( T0H, 1 ) ),
		[offCycles0] "I" (DELAY_OPS( T0L, 5 ) )
	);
}

// Alternative method of transmitting a pixel from r,g,b values
void tp_sendPixel(uint8_t r, uint8_t g , uint8_t b)  {  
	GRBPixel pixel;
	pixel.g = g;          // Neopixel wants colors in green then red then blue order
	pixel.r = r;
	pixel.b = b;
	tp_sendGRBPixel(&pixel);
}

// Just wait long enough without sending any bits to cause the pixels to latch and display the last sent frame
// Use this to terminate a dynamically created output string of pixels
void tp_latch() {
	// ensure bit is low
	PIXEL_PORT &= ~tp_ledPinMask;
	delayMicroseconds(LATCH_USEC);
}

// put GRB struct value into buffer
void tp_setGRBBuffer(uint16_t index, GRBPixel pixel) {
	if(tp_buffer != NULL && index < tp_pixelCount) {
		tp_buffer[index] = pixel;
	}
}

// put rgb pixel values into buffer
void tp_setBuffer(uint16_t index, uint8_t r, uint8_t g , uint8_t b) {
	GRBPixel pixel;
	pixel.g = g;          // Neopixel wants colors in green then red then blue order
	pixel.r = r;
	pixel.b = b;
	tp_setGRBBuffer(index, pixel);
}

// set buffer to a specific rgb value
void tp_setAllBuffer(uint8_t r, uint8_t g , uint8_t b) {
	if(tp_buffer != NULL) {
		uint16_t i;
		GRBPixel pixel;
		pixel.g = g;          // Neopixel wants colors in green then red then blue order
		pixel.r = r;
		pixel.b = b;
		for(i = 0; i < tp_pixelCount; i++) {
			tp_setGRBBuffer(i, pixel);
		}
	}
}

// transmit a pre-consructed buffer of GRBPixels
void tp_sendBuffer() {
	if(tp_buffer != NULL) {
		uint16_t i;
		for(i = 0; i < tp_pixelCount; i++) {
			tp_sendGRBPixel(&tp_buffer[i]);
		}
		tp_latch();
	}
}


// Set the specified pin up as digital out
void tp_ledsetup(uint8_t ledPin, uint16_t pixelCount, bool enableBuffer) {
	tp_pixelCount = pixelCount;
	tp_ledPinMask = 1 << ledPin;
	PIXEL_DDR |= tp_ledPinMask;
	tp_sendPixel(0,0,0);
	tp_latch();
	if(enableBuffer) {
		tp_buffer = (GRBPixel*)malloc(pixelCount * sizeof(GRBPixel));
	} else if(tp_buffer != NULL) {
		free(tp_buffer);
	}
}


