// http://esp8266.github.io/Arduino/versions/2.3.0/doc/filesystem.html#uploading-files-to-file-system
// esp8266com/esp8266/libraries/SD/src/File.cpp
#include <FS.h>

#include "config.h"

#include "GifPlayer.h"
GifPlayer gifPlayer;

#if defined(ESP8266) || defined(ESP32)
extern "C" {
#include "user_interface.h"
}
#else
#error This code uses SPIFFS on ESPxx chips
#endif


void display_resolution() {
    matrix->setTextSize(1);
    // not wide enough;
    if (mw<16) return;
    matrix_clear();
    // Font is 5x7, if display is too small
    // 8 can only display 1 char
    // 16 can almost display 3 chars
    // 24 can display 4 chars
    // 32 can display 5 chars
    matrix->setCursor(0, 0);
    matrix->setTextColor(matrix->Color(255,0,0));
    if (mw>10) matrix->print(mw/10);
    matrix->setTextColor(matrix->Color(255,128,0)); 
    matrix->print(mw % 10);
    matrix->setTextColor(matrix->Color(0,255,0));
    matrix->print('x');
    // not wide enough to print 5 chars, go to next line
    if (mw<25) {
	if (mh==13) matrix->setCursor(6, 7);
	else if (mh>=13) {
	    matrix->setCursor(mw-12, 8);
	} else {
	    // we're not tall enough either, so we wait and display
	    // the 2nd value on top.
	    matrix_show();
	    matrix_clear();
	    matrix->setCursor(mw-11, 0);
	}   
    }
    matrix->setTextColor(matrix->Color(0,255,128)); 
    matrix->print(mh/10);
    matrix->setTextColor(matrix->Color(0,128,255));  
    matrix->print(mh % 10);
    // enough room for a 2nd line
    if ((mw>25 && mh >14) || mh>16) {
	matrix->setCursor(0, mh-7);
	matrix->setTextColor(matrix->Color(0,255,255)); 
	if (mw>16) matrix->print('*');
	matrix->setTextColor(matrix->Color(255,0,0)); 
	matrix->print('R');
	matrix->setTextColor(matrix->Color(0,255,0));
	matrix->print('G');
	matrix->setTextColor(matrix->Color(0,0,255)); 
	matrix->print("B");
	matrix->setTextColor(matrix->Color(255,255,0)); 
	// this one could be displayed off screen, but we don't care :)
	matrix->print("*");
    }
    
    matrix_show();
}

#if 0
/gifs/ChasmDaltynBg.gif, size: 13920
/gifs/ChasmDaltynChest.gif, size: 40786
/gifs/ChasmDaltynCrouch.gif, size: 23545
/gifs/ChasmDaltynFight.gif, size: 23167
/gifs/ChasmDaltynKnife.gif, size: 12091
/gifs/ChasmDaltynPunch.gif, size: 13550
/gifs/ChasmDaltynRun.gif, size: 114519
/gifs/ChasmDaltynShield.gif, size: 26480
/gifs/ChasmDaltynSlash.gif, size: 15036
/gifs/ChasmDaltynSword.gif, size: 13488
/gifs/ChasmDaltynWhip.gif, size: 10938
/gifs/ChasmGoblin.gif, size: 20530
/gifs/ChasmMonster1.gif, size: 6325
/gifs/ChasmMonster2.gif, size: 7917
/gifs/ChasmMonster3.gif, size: 9606
/gifs/ChasmOldMan.gif, size: 9969
/gifs/DinoRun1.gif, size: 1918
/gifs/DinoRun10.gif, size: 2045
/gifs/DinoRun11.gif, size: 1844
/gifs/DinoRun2.gif, size: 3232
/gifs/DinoRun3.gif, size: 3119
/gifs/DinoRun6.gif, size: 2898
/gifs/DinoRun7-2.gif, size: 2207
/gifs/DinoRun7.gif, size: 1686
/gifs/DinoRun9-1.gif, size: 1991
/gifs/DinoRun9-2.gif, size: 2201
/gifs/DinoRun9-3.gif, size: 1474
/gifs/DinoRun9-4.gif, size: 1681
/gifs/TinyFlex.gif, size: 2034
#endif


void loop() {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {
	uint32_t result;

	String fileName = dir.fileName();
	//fileName = "/gifs/DinoRun2.gif";
	//fileName = "/gifs/32anim_balls.gif";
	//fileName = "/gifs/32anim_dance.gif";
	//fileName = "/gifs/32anim_flower.gif";
	//fileName = "/gifs/32anim_photon.gif";

	Serial.print("Reading ");
	Serial.println(fileName);

	File imageFile = SPIFFS.open(fileName, "r");
	if (!imageFile) {
	    Serial.println("Failed to open");
	    return;
	}
	
	gifPlayer.setFile(imageFile);

	for (uint8_t c=0; c<10; c++) {
	    if (!gifPlayer.parseGifHeader()) {
	      imageFile.close();
	      Serial.println("No gif header");
	      return;
	    }

	    matrix_clear();
	    gifPlayer.parseLogicalScreenDescriptor();
	    gifPlayer.parseGlobalColorTable();
	    Serial.println("Processing gif");
	    do {
		gifPlayer.drawFrame();
		result = gifPlayer.drawFrame();
		matrix_show();
		delay(50);
	    } while (result != ERROR_FINISHED);
	    imageFile.seek(0);
	}

	Serial.println("Gif finished");
	imageFile.close();
	delay(1000);
    }
}

void setup() {
    Serial.begin(115200);

#ifdef ESP8266
    Serial.println();
    Serial.print( F("Heap: ") ); Serial.println(system_get_free_heap_size());
    Serial.print( F("Boot Vers: ") ); Serial.println(system_get_boot_version());
    Serial.print( F("CPU: ") ); Serial.println(system_get_cpu_freq());
    Serial.print( F("SDK: ") ); Serial.println(system_get_sdk_version());
    Serial.print( F("Chip ID: ") ); Serial.println(system_get_chip_id());
    Serial.print( F("Flash ID: ") ); Serial.println(spi_flash_get_id());
    Serial.print( F("Flash Size: ") ); Serial.println(ESP.getFlashChipRealSize());
    Serial.print( F("Vcc: ") ); Serial.println(ESP.getVcc());
    Serial.println();
#endif

    SPIFFS.begin();
    {
	Dir dir = SPIFFS.openDir("/");
	while (dir.next()) {
	    String fileName = dir.fileName();
	    size_t fileSize = dir.fileSize();
	    Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), String(fileSize).c_str());
	}
	Serial.printf("\n");
    }

    FastLED.addLeds<WS2811_PORTA,3>(matrixleds, NUMMATRIX/3).setCorrection(TypicalLEDStrip);
    matrix->begin();
    matrix->setBrightness(matrix_brightness);
    display_resolution();
}

// vim:sts=4:sw=4
