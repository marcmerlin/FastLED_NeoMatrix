/*-------------------------------------------------------------------------
  Arduino library based on Adafruit_Neomatrix but modified to work with FastLED
  by Marc MERLIN <marc_soft@merlins.org>

  Original notice and license from Adafruit_Neomatrix:
  Arduino library to control single and tiled matrices of WS2811- and
  WS2812-based RGB LED devices such as the Adafruit NeoPixel Shield or
  displays assembled from NeoPixel strips, making them compatible with
  the Adafruit_GFX graphics library.  Requires both the FastLED_NeoPixel
  and Adafruit_GFX libraries.

  Written by Phil Burgess / Paint Your Dragon for Adafruit Industries.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing products
  from Adafruit!

  -------------------------------------------------------------------------
  This file is part of the Adafruit NeoMatrix library.

  NeoMatrix is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation, either version 3 of
  the License, or (at your option) any later version.

  NeoMatrix is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with NeoMatrix.  If not, see
  <http://www.gnu.org/licenses/>.
  -------------------------------------------------------------------------*/

#include <Adafruit_GFX.h>
#include <FastLED_NeoMatrix.h>
#include <FastLED.h>
#include "gamma.h"
#ifdef __AVR__
 #include <avr/pgmspace.h>
#elif defined(ESP8266)
 #include <pgmspace.h>
#else
 #ifndef pgm_read_byte
  #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
 #endif
#endif

#ifndef _swap_uint16_t
#define _swap_uint16_t(a, b) { uint16_t t = a; a = b; b = t; }
#endif



// Constructor for single matrix:
FastLED_NeoMatrix::FastLED_NeoMatrix(int w, int h, uint8_t pin, uint8_t matrixType, uint8_t ledType): 
  Adafruit_GFX(w, h),
  CFastLED(),
  pin(pin), type(matrixType), ledType(ledType), matrixWidth(w), matrixHeight(h), tilesX(0), tilesY(0), remapFn(NULL) { 
    // WARNING: Serial.print seems to crash in the constructor, but works in begin()
  }

// Constructor for tiled matrices:
FastLED_NeoMatrix::FastLED_NeoMatrix(uint8_t mW, uint8_t mH, uint8_t tX, uint8_t tY, uint8_t pin, uint8_t matrixType, uint8_t ledType) :
  Adafruit_GFX(mW * tX, mH * tY), 
  CFastLED(),
  pin(pin), type(matrixType), ledType(ledType), matrixWidth(mW), matrixHeight(mH), tilesX(tX), tilesY(tY), remapFn(NULL) { 
    numpix = matrixWidth * matrixHeight * tilesX * tilesY;
    //TODO: write me
 }

 void FastLED_NeoMatrix::begin() {
    numpix = matrixWidth * matrixHeight;
    _malloc_size = numpix * sizeof(CRGB);
    Serial.print("Num Pixels: ");
    Serial.println(numpix);
    Serial.print("malloc size: ");
    Serial.println(_malloc_size);
    if (! (leds = (CRGB *) malloc(_malloc_size)))
    {
	while (1) {
	    Serial.println(F("Malloc failed for LED Matrix"));
	}
    }
    // And this, is why templates are forbidden in many companies' style guide.
    // Not only they're so unreadable, but they require that you set their argument
    // value at compilation time, leading to silly stuff like this:
    // TODO: we need even more if statements here to allow for other strip types than
    // just WS1812B, which was the whole point behind this library: support more than 
    // neopixels only.
    if (ledType == NEO_TYPE_NEOPIXEL) {
      if (pin == 1)  addLeds<NEOPIXEL,1>( leds, numpix);
      if (pin == 2)  addLeds<NEOPIXEL,2>( leds, numpix);
      if (pin == 3)  addLeds<NEOPIXEL,3>( leds, numpix);
      if (pin == 4)  addLeds<NEOPIXEL,4>( leds, numpix);
      if (pin == 5)  addLeds<NEOPIXEL,5>( leds, numpix);
#if not defined(ESP32) and not defined(ESP8266)
      if (pin == 6)  addLeds<NEOPIXEL,6>( leds, numpix);
      if (pin == 7)  addLeds<NEOPIXEL,7>( leds, numpix);
      if (pin == 8)  addLeds<NEOPIXEL,8>( leds, numpix);
      if (pin == 9)  addLeds<NEOPIXEL,9>( leds, numpix);
      if (pin == 10) addLeds<NEOPIXEL,10>(leds, numpix);
      if (pin == 11) addLeds<NEOPIXEL,11>(leds, numpix);
#endif
      if (pin == 12) addLeds<NEOPIXEL,12>(leds, numpix);
      if (pin == 13) addLeds<NEOPIXEL,13>(leds, numpix);
      if (pin == 14) addLeds<NEOPIXEL,14>(leds, numpix);
      if (pin == 15) addLeds<NEOPIXEL,15>(leds, numpix);
      if (pin == 16) addLeds<NEOPIXEL,16>(leds, numpix);
      if (pin == 17) addLeds<NEOPIXEL,17>(leds, numpix);
      if (pin == 18) addLeds<NEOPIXEL,18>(leds, numpix);
      if (pin == 19) addLeds<NEOPIXEL,19>(leds, numpix);
#if not defined(ESP32) and not defined(ESP8266)
      if (pin == 20) addLeds<NEOPIXEL,20>(leds, numpix);
#endif
      if (pin == 21) addLeds<NEOPIXEL,21>(leds, numpix);
      if (pin == 22) addLeds<NEOPIXEL,22>(leds, numpix);
      if (pin == 23) addLeds<NEOPIXEL,23>(leds, numpix);
#if not defined(ESP32) and not defined(ESP8266)
      if (pin == 24) addLeds<NEOPIXEL,24>(leds, numpix);
#endif
    }
  }

// Expand 16-bit input color (Adafruit_GFX colorspace) to 24-bit (NeoPixel)
// (w/gamma adjustment)
static uint32_t expandColor(uint16_t color) {
  return ((uint32_t)pgm_read_byte(&gamma5[ color >> 11       ]) << 16) |
         ((uint32_t)pgm_read_byte(&gamma6[(color >> 5) & 0x3F]) <<  8) |
                    pgm_read_byte(&gamma5[ color       & 0x1F]);
}

// Downgrade 24-bit color to 16-bit (add reverse gamma lookup here?)
uint16_t FastLED_NeoMatrix::Color(uint8_t r, uint8_t g, uint8_t b) {
  return ((uint16_t)(r & 0xF8) << 8) |
         ((uint16_t)(g & 0xFC) << 3) |
                    (b         >> 3);
}

// Pass-through is a kludge that lets you override the current drawing
// color with a 'raw' RGB (or RGBW) value that's issued directly to
// pixel(s), side-stepping the 16-bit color limitation of Adafruit_GFX.
// This is not without some limitations of its own -- for example, it
// won't work in conjunction with the background color feature when
// drawing text or bitmaps (you'll just get a solid rect of color),
// only 'transparent' text/bitmaps.  Also, no gamma correction.
// Remember to UNSET the passthrough color immediately when done with
// it (call with no value)!

// Pass raw color value to set/enable passthrough
void FastLED_NeoMatrix::setPassThruColor(uint32_t c) {
  passThruColor = c;
  passThruFlag  = true;
}

// Call without a value to reset (disable passthrough)
void FastLED_NeoMatrix::setPassThruColor(void) {
  passThruFlag = false;
}

void FastLED_NeoMatrix::drawPixel(int16_t x, int16_t y, uint16_t color) {

  if((x < 0) || (y < 0) || (x >= _width) || (y >= _height)) return;

  int16_t t;
  switch(rotation) {
   case 1:
    t = x;
    x = WIDTH  - 1 - y;
    y = t;
    break;
   case 2:
    x = WIDTH  - 1 - x;
    y = HEIGHT - 1 - y;
    break;
   case 3:
    t = x;
    x = y;
    y = HEIGHT - 1 - t;
    break;
  }

  int tileOffset = 0, pixelOffset;

  if(remapFn) { // Custom X/Y remapping function
    pixelOffset = (*remapFn)(x, y);
  } else {      // Standard single matrix or tiled matrices

    uint8_t  corner = type & NEO_MATRIX_CORNER;
    uint16_t minor, major, majorScale;

    if(tilesX) { // Tiled display, multiple matrices
      uint16_t tile;

      minor = x / matrixWidth;            // Tile # X/Y; presume row major to
      major = y / matrixHeight,           // start (will swap later if needed)
      x     = x - (minor * matrixWidth);  // Pixel X/Y within tile
      y     = y - (major * matrixHeight); // (-* is less math than modulo)

      // Determine corner of entry, flip axes if needed
      if(type & NEO_TILE_RIGHT)  minor = tilesX - 1 - minor;
      if(type & NEO_TILE_BOTTOM) major = tilesY - 1 - major;

      // Determine actual major axis of tiling
      if((type & NEO_TILE_AXIS) == NEO_TILE_ROWS) {
        majorScale = tilesX;
      } else {
        _swap_uint16_t(major, minor);
        majorScale = tilesY;
      }

      // Determine tile number
      if((type & NEO_TILE_SEQUENCE) == NEO_TILE_PROGRESSIVE) {
        // All tiles in same order
        tile = major * majorScale + minor;
      } else {
        // Zigzag; alternate rows change direction.  On these rows,
        // this also flips the starting corner of the matrix for the
        // pixel math later.
        if(major & 1) {
          corner ^= NEO_MATRIX_CORNER;
          tile = (major + 1) * majorScale - 1 - minor;
        } else {
          tile =  major      * majorScale     + minor;
        }
      }

      // Index of first pixel in tile
      tileOffset = tile * matrixWidth * matrixHeight;

    } // else no tiling (handle as single tile)

    // Find pixel number within tile
    minor = x; // Presume row major to start (will swap later if needed)
    major = y;

    // Determine corner of entry, flip axes if needed
    if(corner & NEO_MATRIX_RIGHT)  minor = matrixWidth  - 1 - minor;
    if(corner & NEO_MATRIX_BOTTOM) major = matrixHeight - 1 - major;

    // Determine actual major axis of matrix
    if((type & NEO_MATRIX_AXIS) == NEO_MATRIX_ROWS) {
      majorScale = matrixWidth;
    } else {
      _swap_uint16_t(major, minor);
      majorScale = matrixHeight;
    }

    // Determine pixel number within tile/matrix
    if((type & NEO_MATRIX_SEQUENCE) == NEO_MATRIX_PROGRESSIVE) {
      // All lines in same order
      pixelOffset = major * majorScale + minor;
    } else {
      // Zigzag; alternate rows change direction.
      if(major & 1) pixelOffset = (major + 1) * majorScale - 1 - minor;
      else          pixelOffset =  major      * majorScale     + minor;
    }
  }

  leds[tileOffset + pixelOffset] =
    passThruFlag ? passThruColor : expandColor(color);
}

void FastLED_NeoMatrix::fillScreen(uint16_t color) {
  uint32_t c;

  c = passThruFlag ? passThruColor : expandColor(color);
  for (uint16_t i=0; i<numpix; i++) { leds[i]=c; }
}

void FastLED_NeoMatrix::setRemapFunction(uint16_t (*fn)(uint16_t, uint16_t)) {
  remapFn = fn;
}

// vim:sts=2:sw=2
