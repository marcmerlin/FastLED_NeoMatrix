#ifndef config_h
#define config_h
#include <Adafruit_GFX.h>
#include <FastLED_NeoMatrix.h>
#include <FastLED.h>



#ifdef M32B8X3
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

CRGB matrixleds[NUMMATRIX];

FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(matrixleds, MATRIX_TILE_WIDTH, MATRIX_TILE_HEIGHT, MATRIX_TILE_H, MATRIX_TILE_V, 
  NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
    NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG + 
    NEO_TILE_TOP + NEO_TILE_LEFT +  NEO_TILE_PROGRESSIVE);

#else
//---------------------------------------------------------------------------- 
//
// Used by LEDMatrix
#define MATRIX_TILE_WIDTH   64 // width of EACH NEOPIXEL MATRIX (not total display)
#define MATRIX_TILE_HEIGHT  64 // height of each matrix
#define MATRIX_TILE_H       1  // number of matrices arranged horizontally
#define MATRIX_TILE_V       1  // number of matrices arranged vertically
#define NUM_STRIPS 16
#define NUM_LEDS_PER_STRIP 256

// Used by NeoMatrix
#define mw (MATRIX_TILE_WIDTH *  MATRIX_TILE_H)
#define mh (MATRIX_TILE_HEIGHT * MATRIX_TILE_V)
#define NUMMATRIX (mw*mh)

// Compat for some other demos
#define NUM_LEDS NUMMATRIX 
#define MATRIX_HEIGHT mh
#define MATRIX_WIDTH mw
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

CRGB matrixleds[NUMMATRIX];

FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(matrixleds, MATRIX_TILE_WIDTH, MATRIX_TILE_HEIGHT,  
    NEO_MATRIX_BOTTOM + NEO_MATRIX_LEFT +
    NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG );
#endif


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


void matrix_setup() {
#ifdef M32B8X3
    // Init Matrix
    // Serialized, 768 pixels takes 26 seconds for 1000 updates or 26ms per refresh
    // FastLED.addLeds<NEOPIXEL,MATRIXPIN>(matrixleds, NUMMATRIX).setCorrection(TypicalLEDStrip);
    // https://github.com/FastLED/FastLED/wiki/Parallel-Output
    // WS2811_PORTA - pins 12, 13, 14 and 15 or pins 6,7,5 and 8 on the NodeMCU
    // This is much faster 1000 updates in 10sec
    //FastLED.addLeds<NEOPIXEL,PIN>(matrixleds, NUMMATRIX); 
    FastLED.addLeds<WS2811_PORTA,3>(matrixleds, NUMMATRIX/3).setCorrection(TypicalLEDStrip);
    Serial.print("Neomatrix parallel output, total LEDs: ");
    Serial.println(NUMMATRIX);
#else
    // https://github.com/FastLED/FastLED/wiki/Multiple-Controller-Examples
    FastLED.addLeds<WS2812B, 2, GRB>(matrixleds, 0*NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP); 
    FastLED.addLeds<WS2812B, 4, GRB>(matrixleds, 1*NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP); 
    FastLED.addLeds<WS2812B, 5, GRB>(matrixleds, 2*NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP); 
    FastLED.addLeds<WS2812B,12, GRB>(matrixleds, 3*NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP); 
    FastLED.addLeds<WS2812B,13, GRB>(matrixleds, 4*NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP); 
    FastLED.addLeds<WS2812B,14, GRB>(matrixleds, 5*NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP); 
    FastLED.addLeds<WS2812B,15, GRB>(matrixleds, 6*NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP); 
    FastLED.addLeds<WS2812B,16, GRB>(matrixleds, 7*NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP); 
    FastLED.addLeds<WS2812B,18, GRB>(matrixleds, 8*NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP); 
    FastLED.addLeds<WS2812B,19, GRB>(matrixleds, 9*NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP); 
    FastLED.addLeds<WS2812B,21, GRB>(matrixleds,10*NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP); 
    FastLED.addLeds<WS2812B,22, GRB>(matrixleds,11*NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP); 
    FastLED.addLeds<WS2812B,23, GRB>(matrixleds,12*NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP); 
    FastLED.addLeds<WS2812B,25, GRB>(matrixleds,13*NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP); 
    FastLED.addLeds<WS2812B,26, GRB>(matrixleds,14*NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP); 
    FastLED.addLeds<WS2812B,27, GRB>(matrixleds,15*NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
    Serial.print("Neomatrix 16 way output, total LEDs: ");
    Serial.println(NUMMATRIX);
#endif

    FastLED.setBrightness(matrix_brightness);
}

#endif // config.h
