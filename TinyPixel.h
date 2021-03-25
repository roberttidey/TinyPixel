/*
	TinyPixel library
	R.J.Tidey Mar 22 2021
	Based on timing ideas and SimplePixelDemo
	at http://wp.josh.com/2014/05/11/ws2812-neopixels-made-easy/

	Can be used unbuffered to allow very long pixel strings with values constructed on the fly
	or buffered to allow simple updating of individual pixels
*/
#ifndef PORT_LETTER
	#define PORT_LETTER B
#endif

#define _CONCAT(x,y) x##y
#define CONCAT(x,y) _CONCAT(x,y)

#define PIXEL_PORT  CONCAT(PORT,PORT_LETTER)  	// Port of the pin the pixels are connected to
#define PIXEL_DDR   CONCAT(DDR,PORT_LETTER)   	// Port of the pin the pixels are connected to
#define PIXEL_TOGGLE   CONCAT(PIN,PORT_LETTER)  // Use to toggle bit of output port

#include <Arduino.h>

struct GRBPixel {
	uint8_t g;
	uint8_t r;
	uint8_t b;
};

extern void tp_ledsetup(uint8_t ledPin, uint16_t pixelCount, bool enableBuffer);
extern void tp_sendGRBPixel(GRBPixel* pixelPtr);
extern void tp_sendPixel(uint8_t r, uint8_t g , uint8_t b);
extern void tp_latch();
extern void tp_setGRBBuffer(uint16_t index, GRBPixel pixel);
extern void tp_setBuffer(uint16_t index, uint8_t r, uint8_t g , uint8_t b);
extern void tp_setAllBuffer(uint8_t r, uint8_t g , uint8_t b);
extern void tp_sendBuffer();

