
/*
 Demo of TinyPixel library
 R.J.Tidey Mar 22 2021
 Based on timing ideas and SimplePixelDemo
 at http://wp.josh.com/2014/05/11/ws2812-neopixels-made-easy/
*/
#include "TinyPixel.h"

// Display a single color on the whole string
#define LED_PIN 0
#define PIXELS 6  // Number of pixels in the string

void showColor(uint8_t r , uint8_t g , uint8_t b ) {
	for( int p=0; p<PIXELS; p++ ) {
		tp_sendPixel( r , g , b );
	}
	tp_latch();
}

// Fill the dots one after the other with a color
// rewrite to lift the compare out of the loop
void colorWipe(uint8_t r , uint8_t g, uint8_t b, uint16_t wait ) {
	GRBPixel pixel1 = {g,r,b};
	GRBPixel pixel0 = {0,0,0};
	for(uint16_t i=0; i<PIXELS; i+= (PIXELS/60 + 1) ) {
		uint16_t p=0;

		while (p++<=i) {
			tp_sendGRBPixel(&pixel1);
		} 
		while (p++<=PIXELS) {
			tp_sendGRBPixel(&pixel0);
		}
		tp_latch();
		delay(wait);
	}
}

// Theatre-style crawling lights.
// Changes spacing to be dynamic based on string size
#define THEATER_SPACING (PIXELS/20 + 1)
void theaterChase(uint8_t r , uint8_t g, uint8_t b, uint16_t wait ) {
	GRBPixel pixel1 = {g,r,b};
	GRBPixel pixel0 = {0,0,0};
	for (uint8_t j=0; j< 3 ; j++) {  
		for (int q=0; q < THEATER_SPACING ; q++) {
			uint8_t step=0;
			for (int i=0; i < PIXELS ; i++) {
				if (step == q) {
					tp_sendGRBPixel(&pixel1);
				} else {
					tp_sendGRBPixel(&pixel0);
				}
				step++;
				if (step == THEATER_SPACING) step = 0;
			}
			tp_latch();
			delay(wait);
		}
	}
}
                                                                            
void rainbowCycle(uint16_t frames, uint16_t frameAdvance, uint16_t pixelAdvance, uint16_t delayms) {
	// Hue is a number between 0 and 3*256 than defines a mix of r->g->b where
	// hue of 0 = Full red
	// hue of 128 = 1/2 red and 1/2 green
	// hue of 256 = Full Green
	// hue of 384 = 1/2 green and 1/2 blue
	// ...
	uint16_t firstPixelHue = 0;     // Color for the first pixel in the string
	for(uint16_t j=0; j<frames; j++) {                                  
		uint16_t currentPixelHue = firstPixelHue;
		for(uint16_t i=0; i< PIXELS; i++) {
			if (currentPixelHue>=(3*256)) {                  // Normalize back down incase we incremented and overflowed
				currentPixelHue -= (3*256);
			}
			uint8_t phase = currentPixelHue >> 8;
			uint8_t step = currentPixelHue & 0xff;
			switch (phase) {
				case 0: 
					tp_sendPixel( ~step , step ,  0 );
					break;
				case 1: 
					tp_sendPixel( 0 , ~step , step );
					break;
				case 2: 
					tp_sendPixel(  step ,0 , ~step );
					break;
			}
			currentPixelHue+=pixelAdvance;                                      
		} 
		tp_latch();
		delay(delayms);
		firstPixelHue += frameAdvance;
	}
}
  
void detonate(uint8_t r, uint8_t g, uint8_t b, uint16_t startdelayms) {
	while (startdelayms) {
		showColor(r, g, b);      // Flash the color 
		showColor(0, 0 , 0);
		delay(startdelayms);      
		startdelayms =  (startdelayms * 4) / 5 ;           // delay between flashes is halved each time until zero
	}
	// Then we fade to black....
	for(unsigned int fade = 256; fade > 0; fade--) {
		showColor((r * fade) / 256, (g * fade) / 256, (b * fade) / 256);
	}
	showColor(0, 0, 0);
}

// random buffer flash
void randomFlash(uint16_t loop, uint16_t delayms) {
	uint16_t count, pixel;
	uint8_t r,g,b;
	tp_setAllBuffer(0,0,0);
	for(count = 0; count < loop ; count++) {
		pixel = random(0, PIXELS);
		r = random(0, 256);
		g = random(0, 256);
		b = random(0, 256);
		tp_setBuffer(pixel,r,g,b);
		delay(delayms);
		tp_sendBuffer();
	}
}

void setup() {
	tp_ledsetup(LED_PIN, PIXELS, true);  
}

void loop() {
	// Some example procedures showing how to display to the pixels:
	colorWipe(255, 0, 0, 2000 / PIXELS); // Red
	colorWipe(0, 255, 0, 2000 / PIXELS); // Green
	colorWipe(0, 0, 255, 2000 / PIXELS); // Blue
	// Send a theater pixel chase in...
	theaterChase(127, 127, 127, 200); // White
	theaterChase(127, 0, 0, 200); // Red
	theaterChase(0, 0,127, 200); // Blue
	rainbowCycle(100, 20, 5, 30);
	detonate(255, 255, 255, 1000);
	randomFlash(100,50);
}





