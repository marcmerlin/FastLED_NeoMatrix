#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>

#define PIN 6

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 5, PIN,
  NEO_MATRIX_TOP | NEO_MATRIX_LEFT | NEO_MATRIX_ROWS | NEO_MATRIX_PROGRESSIVE +
  NEO_GRB + NEO_KHZ800);

void setup() {
  matrix.begin();
  matrix.setRotation(3);
  matrix.setTextWrap(false);
  matrix.setTextColor(0x4000); // Medium red
}

int x = 6;

void loop() {
  matrix.fillScreen(0);
  matrix.setCursor(x, 0);
  matrix.print("Hello");
  if(--x < -36) x = 6;
  matrix.show();
  delay(100);

  /*
  matrix.drawCircle(2, 2, 2, 0xF800);
  matrix.show();
  delay(1000);
  matrix.drawCircle(2, 2, 2, 0x07E0);
  matrix.show();
  delay(1000);
  matrix.drawCircle(2, 2, 2, 0x001F);
  */
}

