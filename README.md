# TinyPixel

- This library drives WS2812B pixels
- Developed for ATTiny but can be used on other avr platforms
- Uses relaxed timing based on http://wp.josh.com/2014/05/11/ws2812-neopixels-made-easy/
- Can be used non buffered with pixel data generated dynamically allowing very long pixel strings with low memory used
- Can also be used in buffered mode to simplify changing individual pixel data but requires 3 bytes of RAM per pixel
- Simple api
- Interrupts are left on for all but a 1 uSec critical piece of code.
- Older WS2812 may require other interrupts to be less than 5uS
- Newer WS2812B allow interruptions up to 200uSec or more

### Functions

- void tp_ledsetup(uint8_t ledPin, uint16_t pixelCount, bool enableBuffer);
- void tp_sendGRBPixel(GRBPixel* pixelPtr);
- void tp_sendPixel(uint8_t r, uint8_t g , uint8_t b);
- void tp_latch();
- void tp_setGRBBuffer(uint16_t index, GRBPixel pixel);
- void tp_setBuffer(uint16_t index, uint8_t r, uint8_t g , uint8_t b);
- void tp_setAllBuffer(uint8_t r, uint8_t g , uint8_t b);
- void tp_sendBuffer();

### Config and used
- Sketch should define number of pixels, pin and port to be used
- port defaults to PORTB for ATTiny use, Use #define PORT_LETTER B at top of sketch to override
- Use tp_ledsetp to initialise
- For non buffered mode send a sequence of tp_sendGRBPixel or tp_sendPixel for each pixel in the strip
- terminate the sequence with tp_latch
- For buffered mode use setALLBuffer, setBuffer, and setGRBBuffer to define or modify pixels in the buffered
- Use tp_sendBuffer to transmit buffer contents
