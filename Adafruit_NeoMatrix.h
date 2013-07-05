#ifndef _ADAFRUIT_NEOMATRIX_H_
#define _ADAFRUIT_NEOMATRIX_H_

#if ARDUINO >= 100
 #include <Arduino.h>
#else
 #include <WProgram.h>
 #include <pins_arduino.h>
#endif
#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>

// Matrix layout parameters are passed in the 16-bit 'type' parameter for
// each constructor.  The low-order byte contains NeoPixel-specific data
// from the NeoPixel library (e.g. NEO_GRB or NEO_KHZ800), the high-order
// byte contains NeoMatrix data defined here (matrix layout, etc.).  Add
// the two to produce a final type value for the constructor.

// These define the layout for a single 'unified' matrix (e.g. one made
// from NeoPixel strips, or a single NeoPixel shield), or for the pixels
// within each matrix of a tiled display (e.g. multiple NeoPixel shields).

#define NEO_MATRIX_TOP         0x0000 // Pixel 0 is at top of matrix
#define NEO_MATRIX_BOTTOM      0x0100 // Pixel 0 is at bottom of matrix
#define NEO_MATRIX_LEFT        0x0000 // Pixel 0 is at left of matrix
#define NEO_MATRIX_RIGHT       0x0200 // Pixel 0 is at right of matrix
#define NEO_MATRIX_CORNER      0x0300 // Bitmask for pixel 0 matrix corner
#define NEO_MATRIX_ROWS        0x0000 // Matrix is row major (horizontal)
#define NEO_MATRIX_COLUMNS     0x0400 // Matrix is column major (vertical)
#define NEO_MATRIX_AXIS        0x0400 // Bitmask for row/column layout
#define NEO_MATRIX_PROGRESSIVE 0x0000 // Same pixel order across each line
#define NEO_MATRIX_ZIGZAG      0x0800 // Pixel order reverses between lines
#define NEO_MATRIX_SEQUENCE    0x0800 // Bitmask for pixel line order

// These apply only to tiled displays (multiple matrices):

#define NEO_TILE_TOP           0x0000 // First tile is at top of matrix
#define NEO_TILE_BOTTOM        0x1000 // First tile is at bottom of matrix
#define NEO_TILE_LEFT          0x0000 // First tile is at left of matrix
#define NEO_TILE_RIGHT         0x2000 // First tile is at right of matrix
#define NEO_TILE_CORNER        0x3000 // Bitmask for first tile corner
#define NEO_TILE_ROWS          0x0000 // Tiles ordered in rows
#define NEO_TILE_COLUMNS       0x4000 // Tiles ordered in columns
#define NEO_TILE_AXIS          0x4000 // Bitmask for tile H/V orientation
#define NEO_TILE_PROGRESSIVE   0x0000 // Same tile order across each line
#define NEO_TILE_ZIGZAG        0x8000 // Tile order reverses between lines
#define NEO_TILE_SEQUENCE      0x8000 // Bitmas for tile line order

class Adafruit_NeoMatrix : public Adafruit_GFX, public Adafruit_NeoPixel {

 public:

  // Constructor for single matrix:
  Adafruit_NeoMatrix(int w, int h, uint8_t pin=6, uint16_t type=
    NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_GRB + NEO_KHZ800);

  // Constructor for tiled matrices:
  Adafruit_NeoMatrix(uint8_t matrixW, uint8_t matrixH, uint8_t tilesX,
    uint8_t tilesY, uint8_t pin=6, uint16_t type=NEO_MATRIX_TOP +
    NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_TILE_TOP + NEO_TILE_LEFT +
    NEO_TILE_ROWS + NEO_GRB + NEO_KHZ800);

  void
    drawPixel(int16_t x, int16_t y, uint16_t color);

 private:

  uint16_t type,
           (*remapFn)(uint16_t x, uint16_t y);
  boolean  tiled;
  uint8_t  matrixWidth, matrixHeight, tileX, tileY;

#ifdef SLARTIBARTFAST

  // Stuff not yet implemented
  void
    fillScreen(uint16_t c),
    dumpMatrix(void);
  uint16_t
    Color888(uint8_t r, uint8_t g, uint8_t b),
    Color888(uint8_t r, uint8_t g, uint8_t b, boolean gflag),
    ColorHSV(long hue, uint8_t sat, uint8_t val, boolean gflag);

 private:

#endif
};

#endif // _ADAFRUIT_NEOMATRIX_H_
