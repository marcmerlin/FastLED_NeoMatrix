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
    pattern.drawFrame();
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
    pattern.start();
}

// vim:sts=4:sw=4
