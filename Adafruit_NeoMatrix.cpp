#include <Adafruit_NeoPixel.h>
#include <Adafruit_NeoMatrix.h>
#include "gamma.h"
#ifdef __AVR__
 #include <avr/pgmspace.h>
#else
 #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif

// Constructor for single matrix:
Adafruit_NeoMatrix::Adafruit_NeoMatrix(int w, int h, uint8_t pin,
  uint8_t matrixType, uint8_t ledType) : Adafruit_GFX(w, h),
  Adafruit_NeoPixel(w * h, pin, ledType), type(matrixType), matrixWidth(w),
  matrixHeight(h), tilesX(0), tilesY(0), remapFn(NULL) { }

// Constructor for tiled matrices:
Adafruit_NeoMatrix::Adafruit_NeoMatrix(uint8_t mW, uint8_t mH, uint8_t tX,
  uint8_t tY, uint8_t pin, uint8_t matrixType, uint8_t ledType) :
  Adafruit_GFX(mW * tX, mH * tY), Adafruit_NeoPixel(mW * mH * tX * tY, pin,
  ledType), type(matrixType), matrixWidth(mW), matrixHeight(mH), tilesX(tX),
  tilesY(tY), remapFn(NULL) { }

// Expand 16-bit input color (Adafruit_GFX colorspace) to 24-bit (NeoPixel)
// (w/gamma adjustment)
static uint32_t expandColor(uint16_t color) {
  return ((uint32_t)pgm_read_byte(&gamma5[ color >> 11       ]) << 16) |
         ((uint32_t)pgm_read_byte(&gamma6[(color >> 5) & 0x3F]) <<  8) |
                    pgm_read_byte(&gamma5[ color       & 0x1F]);
}

// Downgrade 24-bit color to 16-bit (add reverse gamma lookup here?)
uint16_t Adafruit_NeoMatrix::Color(uint8_t r, uint8_t g, uint8_t b) {
  return ((uint16_t)(r & 0x1f) << 11) |
         ((uint16_t)(g & 0x3f) <<  5) |
                    (b          >> 3);
}

void Adafruit_NeoMatrix::drawPixel(int16_t x, int16_t y, uint16_t color) {

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

  int i;

  // LOTS OF MOJO STILL TO DO HERE
  if(remapFn) {
    // Custom X/Y remapping function
    i = (*remapFn)(x, y);
  } else {
    if(tilesX) {
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

  setPixelColor(i, expandColor(color));
}

void Adafruit_NeoMatrix::fillScreen(uint16_t color) {
  uint16_t i, n;
  uint32_t c24;

  c24 = expandColor(color);
  n   = numPixels();
  for(i=0; i<n; i++) setPixelColor(i, c24);
}

void Adafruit_NeoMatrix::setRemapFunction(uint16_t (*fn)(uint16_t, uint16_t)) {
  remapFn = fn;
}

