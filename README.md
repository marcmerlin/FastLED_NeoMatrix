FastLED NeoMatrix: Matrix with FastLED using Adafruit::GFX API
==============================================================

Blog post with details and pictures:
http://marc.merlins.org/perso/arduino/post_2018-04-23_FastLED_NeoMatrix-library_-how-to-do-Matrices-with-FastLED-and-Adafruit_GFX.html

Adafruit_GFX and FastLED-compatible library for NeoPixel matrices and grids. Controls single and tiled NeoPixel displays.  
This library requires FastLED and Adafruit_GFX libraries as well as this base class library:
- https://github.com/marcmerlin/Framebuffer_GFX
Please look at the Framebuffer_GFX page for details on how the APIs work and you can also look the example demo code:
- https://github.com/marcmerlin/FastLED_NeoMatrix/blob/master/examples/MatrixGFXDemo/MatrixGFXDemo.ino

### Adafruit::NeoMatrix vs FastLED::NeoMatrix

This code was taken from Adafruit_NeoMatrix and adapted to work with the more powerful FastLED library.
The reasons for using FastLED instead of Adafruit::Neopixel as a backend, include:
* FastLED supports more microcontrollers
* Better support for ESP32 than Adafruit::Neopixel
* Support for parallel output on some chips for faster refresh rate: https://github.com/FastLED/FastLED/wiki/Parallel-Output
* Support for interrupts to allow things like Infrared to work while updating strips
* Support for many more led/pixel hardware: https://github.com/FastLED/FastLED/wiki/Chipset-reference
* Once you write code for this library, you can trivially adapt it on any other display hardware supported by https://github.com/marcmerlin/Framebuffer_GFX (like RGBPanels, TFTs, etc...).

How to use the API? It's very close to Adafruit_NeoMatrix, to see an example, you can compare  
https://github.com/adafruit/Adafruit_NeoMatrix/blob/master/examples/MatrixGFXDemo/MatrixGFXDemo.ino
with  
https://github.com/marcmerlin/FastLED_NeoMatrix/blob/master/examples/MatrixGFXDemo/MatrixGFXDemo.ino

You can find a lot of demo code here:
https://github.com/marcmerlin/FastLED_NeoMatrix_SmartMatrix_LEDMatrix_GFX_Demos as well as
a big integrated demo here: https://github.com/marcmerlin/NeoMatrix-FastLED-IR  
Most my demos rely on this big file that lets you configure your panel(s) type and size and have it work with all the demo code. It allows using the exact same code on neopixel arrays, rgbpanels, tft screens, and even your linux computer.

This FastLED_NeoMatrix requires:
- https://github.com/marcmerlin/Framebuffer_GFX (base class)
- https://github.com/adafruit/Adafruit-GFX-Library
- https://github.com/FastLED/FastLED  
- https://github.com/marcmerlin/LEDMatrix is optional if you have code that uses that API


### Tiled Matrixes, Zig-Zag and so forth

See Adafruit's NeoMatrix description, which this lib is fully compatible, with: https://learn.adafruit.com/adafruit-neopixel-uberguide/neomatrix-library
![image](https://user-images.githubusercontent.com/1369412/74280192-202e4300-4cd1-11ea-945c-a06dbdcecb96.png)

### Custom Mappings (non rectangular array)

If you have a custom made design where the pixels are not in a rectangular pattern, you need to provide a myRemapFn which is then fed to the matrix with "matrix->setRemapFunction(myRemapFn)"  
See https://github.com/marcmerlin/FastLED_NeoMatrix/issues/6 for an example.

### Demos

Video demo: https://www.youtube.com/watch?v=tU_wkrrv_4A

![164_20170424_adafruit_gfx_on_neomatrix_32x32](https://user-images.githubusercontent.com/1369412/38774532-5d6b0f2e-4020-11e8-86ef-afdffbeb1e1d.jpg)
![171_20170424_adafruit_gfx_on_neomatrix_32x32](https://user-images.githubusercontent.com/1369412/38774533-5d83d6bc-4020-11e8-95bb-417368143d70.jpg)
