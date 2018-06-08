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

    // Init Matrix
    // Serialized, 768 pixels takes 26 seconds for 1000 updates or 26ms per refresh
    // FastLED.addLeds<NEOPIXEL,MATRIXPIN>(matrixleds, NUMMATRIX).setCorrection(TypicalLEDStrip);
    // https://github.com/FastLED/FastLED/wiki/Parallel-Output
    // WS2811_PORTA - pins 12, 13, 14 and 15 or pins 6,7,5 and 8 on the NodeMCU
    // This is much faster 1000 updates in 10sec
    //FastLED.addLeds<NEOPIXEL,PIN>(matrixleds, NUMMATRIX); 
    FastLED.addLeds<WS2811_PORTA,3>(matrixleds, NUMMATRIX/3).setCorrection(TypicalLEDStrip);
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
