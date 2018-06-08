/*
 * Aurora: https://github.com/pixelmatix/aurora
 * Copyright (c) 2014 Jason Coon
 *
 * Portions of this code are adapted from "Rainbow Smoke" by Jozsef Fejes: http://codegolf.stackexchange.com/a/22326
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#ifndef PatternRainbowSmoke_H
#define PatternRainbowSmoke_H

#include "matrix.h"
#define rgb24 CRGB

class PatternRainbowSmoke : public Drawable {
  private:
    struct Point {
      uint8_t x = 0;
      uint8_t y = 0;
    };

    static const uint8_t NUMCOLORS = 11;
    static const uint16_t COLOR_COUNT = 1024;
    uint8_t startx = 15;
    uint8_t starty = 15;

    rgb24 colors[COLOR_COUNT];
    bool hasColor[MATRIX_WIDTH][MATRIX_HEIGHT];
    bool isAvailable[MATRIX_WIDTH][MATRIX_HEIGHT];

    uint16_t currentColorIndex = 0;
    uint8_t algorithm;

    int colorDifference(rgb24 c1, rgb24 c2) {
      int r = c1.red - c2.red;
      int g = c1.green - c2.green;
      int b = c1.blue - c2.blue;
      return r * r + g * g + b * b;
    }

    void markAvailableNeighbors(Point point) {
      for (int dy = -1; dy <= 1; dy++) {
        int ny = point.y + dy;

        if (ny == -1 || ny == MATRIX_HEIGHT)
          continue;

        for (int dx = -1; dx <= 1; dx++) {
          if (dx == 0 && dy == 0)
            continue;

          int nx = point.x + dx;

          if (nx == -1 || nx == MATRIX_WIDTH)
            continue;

          if (!hasColor[nx][ny]) {
            isAvailable[nx][ny] = true;
          }
        }
      }
    }

    Point getAvailablePoint(int algorithm, rgb24 color) {
      switch (algorithm) {
        case 0:
          return getAvailablePointWithClosestNeighborColor(color);
        case 1:
        default:
          return getAvailablePointWithClosestAverageNeighborColor(color);
      }
    }

    Point getAvailablePointWithClosestNeighborColor(rgb24 color) {
      Point best;

      // find the pixel with the smallest difference between the current color and all of it's neighbors' colors
      int smallestDifference = 999999;
      for (int y = 0; y < MATRIX_HEIGHT; y++) {
        for (int x = 0; x < MATRIX_WIDTH; x++) {
          // skip any that arent' available
          if (!isAvailable[x][y])
            continue;

          // loop through its neighbors
          int smallestDifferenceAmongNeighbors = 999999;
          for (int dy = -1; dy <= 1; dy++) {
            if (y + dy == -1 || y + dy == MATRIX_HEIGHT)
              continue;

            for (int dx = -1; dx <= 1; dx++) {
              if (x == 0 && y == 0)
                continue;

              if (x + dx == -1 || x + dx == MATRIX_WIDTH)
                continue;

              int nx = x + dx;
              int ny = y + dy;

              // skip any neighbors that don't already have a color
              if (!hasColor[nx][ny])
                continue;

              //rgb24 neighborColor = backgroundLayer.readPixel(nx, ny);
              rgb24 neighborColor = matrixleds[XY(nx, ny)];

              int difference = colorDifference(neighborColor, color);
              if (difference < smallestDifferenceAmongNeighbors || (difference == smallestDifferenceAmongNeighbors && random(2) == 1)) {
                smallestDifferenceAmongNeighbors = difference;
              }
            }
          }

          if (smallestDifferenceAmongNeighbors < smallestDifference || (smallestDifferenceAmongNeighbors == smallestDifference && random(2) == 1)) {
            smallestDifference = smallestDifferenceAmongNeighbors;
            best.x = x;
            best.y = y;
          }
        }
      }

      return best;
    }

    Point getAvailablePointWithClosestAverageNeighborColor(rgb24 color) {
      Point best;

      int smallestAverageDifference = 999999;

      for (int y = 0; y < MATRIX_HEIGHT; y++) {
        for (int x = 0; x < MATRIX_WIDTH; x++) {
          // skip any that arent' available
          if (!isAvailable[x][y])
            continue;

          int neighborCount = 0;
          int neighborColorDifferenceTotal = 0;

          // loop through its neighbors
          for (int dy = -1; dy <= 1; dy++) {
            if (y + dy == -1 || y + dy == MATRIX_HEIGHT)
              continue;

            for (int dx = -1; dx <= 1; dx++) {
              if (x + dx == -1 || x + dx == MATRIX_WIDTH)
                continue;

              int nx = x + dx;
              int ny = y + dy;

              // skip any neighbors that don't already have a color
              if (!hasColor[nx][ny])
                continue;

              neighborCount++;

              //rgb24 neighborColor = backgroundLayer.readPixel(nx, ny);
              rgb24 neighborColor = matrixleds[XY(nx, ny)];

              int difference = colorDifference(neighborColor, color);
              neighborColorDifferenceTotal += difference;
            }
          }

          int averageDifferenceAmongNeighbors = neighborColorDifferenceTotal / neighborCount;

          if (averageDifferenceAmongNeighbors < smallestAverageDifference || (averageDifferenceAmongNeighbors == smallestAverageDifference && random(2) == 1)) {
            smallestAverageDifference = averageDifferenceAmongNeighbors;
            best.x = x;
            best.y = y;
          }
        }
      }

      return best;
    }

    void createPalette() {
      int colorSort = random(4);

      switch (colorSort) {
        case 0:
          createPaletteRGB();
          shuffleColors();
          break;
        case 1:
          createPaletteGBR();
          shuffleColors();
          break;
        case 2:
          createPaletteBRG();
          shuffleColors();
          break;
        case 3:
          createPaletteHSV();
          break;
      }
    }

    void createPaletteRGB() {
      int i = 0;

      for (int b = 0; b < NUMCOLORS; b++) {
        for (int g = 0; g < NUMCOLORS; g++) {
          for (int r = 0; r < NUMCOLORS; r++) {
            rgb24 color;
            color.red = r * 255 / (NUMCOLORS - 1);
            color.green = g * 255 / (NUMCOLORS - 1);
            color.blue = b * 255 / (NUMCOLORS - 1);
            colors[i] = color;

            i++;
            if (i == COLOR_COUNT)
              return;
          }
        }
      }
    }

    void createPaletteGBR() {
      int i = 0;

      for (int r = 0; r < NUMCOLORS; r++) {
        for (int b = 0; b < NUMCOLORS; b++) {
          for (int g = 0; g < NUMCOLORS; g++) {
            rgb24 color;
            color.red = r * 255 / (NUMCOLORS - 1);
            color.green = g * 255 / (NUMCOLORS - 1);
            color.blue = b * 255 / (NUMCOLORS - 1);
            colors[i] = color;

            i++;
            if (i == COLOR_COUNT)
              return;
          }
        }
      }
    }

    void createPaletteBRG() {
      int i = 0;

      for (int r = 0; r < NUMCOLORS; r++) {
        for (int g = 0; g < NUMCOLORS; g++) {
          for (int b = 0; b < NUMCOLORS; b++) {
            rgb24 color;
            color.red = r * 255 / (NUMCOLORS - 1);
            color.green = g * 255 / (NUMCOLORS - 1);
            color.blue = b * 255 / (NUMCOLORS - 1);
            colors[i] = color;

            i++;
            if (i == COLOR_COUNT)
              return;
          }
        }
      }
    }

    void shuffleColors() {
      for (int a = 0; a < COLOR_COUNT; a++)
      {
        int r = random(a, COLOR_COUNT);
        rgb24 temp = colors[a];
        colors[a] = colors[r];
        colors[r] = temp;
      }
    }

    void createPaletteHSV() {
      int i = 0;

      uint8_t startHue = random(0, 255);

      for (uint8_t h = startHue; i < 1024; h += 8) {
        for (uint16_t s = 0; s < 256; s += 16) {
          if (i < COLOR_COUNT)
            colors[i] = effects.HsvToRgb(h, s, 255);

          i++;
        }

        for (uint16_t v = 256; v > 0; v -= 16) {
          if (i < COLOR_COUNT)
            colors[i] = effects.HsvToRgb(h, 255, v);

          i++;
        }
      }
    }

  public:
    PatternRainbowSmoke() {
      name = (char *)"RainbowSmoke";
    }

    unsigned int drawFrame() {
      if (currentColorIndex == 0) {
        //randomSeed(analogRead(5));

        //backgroundLayer.fillScreen({ 0, 0, 0 });
        matrix_clear();
        createPalette();
        algorithm = random(2);

        // clear all flags
        for (int y = 0; y < MATRIX_HEIGHT; y++) {
          for (int x = 0; x < MATRIX_WIDTH; x++) {
            hasColor[x][y] = false;
            isAvailable[x][y] = false;
          }
        }
      }

      rgb24 color = colors[currentColorIndex];
      Point point;

      if (currentColorIndex == 0) {
        // use a random starting point
        point.x = random(32);
        point.y = random(32);
      }
      else {
        point = getAvailablePoint(algorithm, color);
      }

      isAvailable[point.x][point.y] = false;
      hasColor[point.x][point.y] = true;

      //backgroundLayer.drawPixel(point.x, point.y, color);
      //effects.Pixel(point.x, point.y, color);
      matrix->setPassThruColor(color.r*65536+color.g*256+color.b);
      matrix->drawPixel(point.x, point.y, color);

      markAvailableNeighbors(point);

      currentColorIndex++;
      if (currentColorIndex >= COLOR_COUNT) {
        currentColorIndex = 0;
        return 3000;
      }

      return 0;
    }

    void start() {
      //backgroundLayer.fillScreen({ 0, 0, 0 });
      matrix_clear();
      currentColorIndex = 0;
    }
};

#endif
