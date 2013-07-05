#include <Adafruit_NeoPixel.h>
#include <Adafruit_NeoMatrix.h>
#include "gamma.h"
#ifdef __AVR__
 #include <avr/pgmspace.h>
#else
 #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif

// Constructor for single matrix:
Adafruit_NeoMatrix::Adafruit_NeoMatrix(int w, int h, uint8_t pin, uint16_t t) : Adafruit_GFX(w, h), Adafruit_NeoPixel(w * h, pin, t) {
  type  = t;
  tiled = false;
}

// Constructor for tiled matrices:
Adafruit_NeoMatrix::Adafruit_NeoMatrix(uint8_t mW, uint8_t mH, uint8_t tX,
  uint8_t tY, uint8_t pin, uint16_t t) : Adafruit_GFX(mW * tX, mH * tY),
  Adafruit_NeoPixel(mW * mH * tX * tY, pin, t) {
  type         = t;
  tiled        = true;
  matrixWidth  = mW;
  matrixHeight = mH;
  tileX        = tX;
  tileY        = tY;
  remapFn      = NULL;
}

void Adafruit_NeoMatrix::drawPixel(int16_t x, int16_t y, uint16_t c) {

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

  uint8_t r, g, b;
  int     i;
  // rrrrrggggggbbbbb
  r = pgm_read_byte(&gamma5[c >> 11]);
  g = pgm_read_byte(&gamma6[((c >> 5) & 0x3F)]);
  b = pgm_read_byte(&gamma5[(c & 0x1F)]);

  if(remapFn) {
    // Custom remapping function
    i = (*remapFn)(x, y);
  } else {
    if(tiled) {
      // Determine tile number
      if((type & NEO_TILE_AXIS) == NEO_TILE_ROWS) {
        // Tile order is row major
      } else {
        // Tile order is column major
      }
    } else {
      // No tiling -- simpler math
      if((type & NEO_MATRIX_AXIS) == NEO_MATRIX_ROWS) {
        // Row major
        if((type & NEO_MATRIX_SEQUENCE) == NEO_MATRIX_PROGRESSIVE) {
          // All rows in same order
          i = y * WIDTH + x;
        } else {
          // Zigzag; alternate rows switch direction
          if(y & 1) i = y * WIDTH + WIDTH - 1 - x; // Odd row
          else      i = y * WIDTH + x;             // Even row
        }
      } else {
        // Column major
      }
    }
  }

  setPixelColor(i, r, g, b);
}
