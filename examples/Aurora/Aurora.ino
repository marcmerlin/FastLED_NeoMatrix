#include "matrix.h"
#include "Effects.h"
#include "Drawable.h"
#include "Boid.h"
#include "Attractor.h"
#include "Geometry.h"

#include "PatternAttract.h"
PatternAttract attract;
#include "PatternBounce.h"
PatternBounce bounce;
#include "PatternCube.h"
PatternCube cube;
#include "PatternFlock.h"
PatternFlock flock;
#include "PatternFlowField.h"
PatternFlowField flowfield;
#include "PatternIncrementalDrift.h"
PatternIncrementalDrift incrementaldrift;
#include "PatternIncrementalDrift2.h"
PatternIncrementalDrift2 incrementaldrift2;
#include "PatternPendulumWave.h"
PatternPendulumWave pendulumwave;
#include "PatternRadar.h"
PatternRadar radar;
#include "PatternSpiral.h"
PatternSpiral spiral;
#include "PatternSpiro.h"
PatternSpiro spiro;
#include "PatternSwirl.h"
PatternSwirl swirl;
#include "PatternWave.h"
PatternWave wave;

Drawable* items[] = {
    &attract,
    &bounce,
    &cube,
    &flock,
    &flowfield,
    &incrementaldrift,
    &incrementaldrift2,
    &pendulumwave,
    &radar,
    &spiral,
    &spiro,
    &swirl,
    &wave,
};
Drawable *pattern;

int8_t item = -1;
uint8_t numitems = sizeof(items) / sizeof(items[0]);


void matrix_show() {
    // On my own code, this gets diverted to
    //FastLED[1].showLeds(matrix_brightness);
    matrix->show();
}

// Parallel output on ESP8266 does not clear seconary panels
void matrix_clear() {
    //FastLED[1].clearLedData();
    // clear does not work properly with multiple matrices connected via parallel inputs
    memset(matrixleds, 0, NUMMATRIX*3);
}

void loop() {
    int8_t new_pattern = 0;
    char readchar;

    if (Serial.available()) readchar = Serial.read(); else readchar = 0;
    if (readchar) {
	while ((readchar >= '0') && (readchar <= '9')) {
	    new_pattern = 10 * new_pattern + (readchar - '0');
	    readchar = 0;
	    if (Serial.available()) readchar = Serial.read();
	}

	if (new_pattern) {
	    Serial.print("Got new pattern via serial ");
	    Serial.println(new_pattern);
	    item = new_pattern;
	} else {
	    Serial.print("Got serial char ");
	    Serial.println(readchar);
	}
    }
    if (readchar == 'n')      { Serial.println("Serial => next"); new_pattern = 1;  item++;}
    else if (readchar == 'p') { Serial.println("Serial => previous"); new_pattern = 1; item--;}

    EVERY_N_SECONDS(40) {
	new_pattern = 1;
	item++;
    }

    if (new_pattern || item == -1) { 
	if (item >= numitems) item = 0;
	if (item <= -1) item = numitems - 1;
	pattern = items[item];
	pattern->start();
	Serial.print("Switching to pattern #");
	Serial.print(item);
	Serial.print(": ");
	Serial.println(pattern->name);
    }
    pattern->drawFrame();
    matrix_show();
}


void setup() {
    delay(1000);
    Serial.begin(115200);

    matrix_setup();
    Serial.print("Matrix Size: ");
    Serial.print(mw);
    Serial.print(" ");
    Serial.println(mh);
    matrix->begin();
    matrix->setBrightness(matrix_brightness);
    matrix->setTextWrap(false);

    effects.leds = matrixleds;
    effects.Setup();
}

// vim:sts=4:sw=4
