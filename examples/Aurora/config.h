#ifndef config_h
#define config_h
#include <Adafruit_GFX.h>
#include <FastLED_NeoMatrix.h>
#include <FastLED.h>

#include <LEDMatrix.h>
#include <LEDSprites.h>

// Fonts + Gifs
// Sketch uses 676884 bytes (64%) of program storage space. Maximum is 1044464 bytes.
// Global variables use 33124 bytes (40%) of dynamic memory, leaving 48796 bytes for local variables. Maximum is 81920 bytes.
// Uploading 681024 bytes from /tmp/arduino_build_498793/NeoMatrix-FastLED-IR.ino.bin to flash at 0x00000000


// disable animated gifs for faster builds/uploads
// Sketch uses 332208 bytes (31%) of program storage space. Maximum is 1044464 bytes.
// Global variables use 33112 bytes (40%) of dynamic memory, leaving 48808 bytes for local variables. Maximum is 81920 bytes.
// Uploading 336352 bytes from /tmp/arduino_build_498793/NeoMatrix-FastLED-IR.ino.bin to flash at 0x00000000
//#define NOANIMGIF 1


// Disable fonts in many sizes
// Sketch uses 283784 bytes (27%) of program storage space. Maximum is 1044464 bytes.
// Global variables use 32880 bytes (40%) of dynamic memory, leaving 49040 bytes for local variables. Maximum is 81920 bytes.
// Uploading 287936 bytes from /tmp/arduino_build_498793/NeoMatrix-FastLED-IR.ino.bin to flash at 0x00000000
//#define NOFONTS 1


//---------------------------------------------------------------------------- 
//
// Used by LEDMatrix
#define MATRIX_TILE_WIDTH   8 // width of EACH NEOPIXEL MATRIX (not total display)
#define MATRIX_TILE_HEIGHT  32 // height of each matrix
#define MATRIX_TILE_H       3  // number of matrices arranged horizontally
#define MATRIX_TILE_V       1  // number of matrices arranged vertically

// Used by NeoMatrix
#define mw (MATRIX_TILE_WIDTH *  MATRIX_TILE_H)
#define mh (MATRIX_TILE_HEIGHT * MATRIX_TILE_V)
#define NUMMATRIX (mw*mh)

// Compat for some other demos
#define NUM_LEDS NUMMATRIX 
#define MATRIX_HEIGHT mh
#define MATRIX_WIDTH mw
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

cLEDMatrix<-MATRIX_TILE_WIDTH, -MATRIX_TILE_HEIGHT, HORIZONTAL_ZIGZAG_MATRIX, MATRIX_TILE_H, MATRIX_TILE_V, HORIZONTAL_BLOCKS> ledmatrix;

//
//CRGB matrixleds[NUMMATRIX];
// cLEDMatrix creates a FastLED array and we need to retrieve a pointer to its first element
// to act as a regular FastLED array.
CRGB *matrixleds = ledmatrix[0];

FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(matrixleds, MATRIX_TILE_WIDTH, MATRIX_TILE_HEIGHT, MATRIX_TILE_H, MATRIX_TILE_V, 
  NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
    NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG + 
    NEO_TILE_TOP + NEO_TILE_LEFT +  NEO_TILE_PROGRESSIVE);


uint8_t matrix_brightness = 32;

int XY2( int x, int y, bool wrap=false) { 
	return matrix->XY(x,MATRIX_HEIGHT-1-y);
}


uint16_t XY( uint8_t x, uint8_t y) {
    return matrix->XY(x,y);
}

#ifdef ESP8266
#define NEOPIXEL_PIN D1 // GPIO5


// D4 is also the system LED, causing it to blink on IR receive, which is great.
#define RECV_PIN D4     // GPIO2

// Turn off Wifi in setup()
// https://www.hackster.io/rayburne/esp8266-turn-off-wifi-reduce-current-big-time-1df8ae
//
#include "ESP8266WiFi.h"
extern "C" {
#include "user_interface.h"
}
// min/max are broken by the ESP8266 include
#define min(a,b) (a<b)?(a):(b)
#define max(a,b) (a>b)?(a):(b)
#endif

bool matrix_reset_demo = 1;

uint8_t gHue = 0; // rotating "base color" used by many of the patterns
uint16_t speed = 255;

int wrapX(int x) { 
	if (x < 0 ) return 0;
	if (x >= MATRIX_WIDTH) return (MATRIX_WIDTH-1);
	return x;
}

void matrix_clear();
void matrix_show();
void aurora_setup();
#endif
