// FireworksXY
// Quick and dirty 2-D fireworks simulation using FastLED.
//
// Originaly designed for an Adafruit 5x8 WS2811 shield, but works fine
// on other XY matricies.
//
// by Mark Kriegsman, July 2013
//
// Updated by JP Roy to have multiple shell launches 2015
//
// Updated by Sublime for Novel Mutations Costume Controllers 2018
//
// Further adapted by Marc MERLIN for integration in FastLED::NeoMatrix
// standalone examples.

#include "config.h"

CRGB& setXY( int x, int y) 
{
	return matrixleds[XY2(wrapX(x),y)];
}

void screenscale( accum88 hor, accum88 ver, fract8& pixel, fract8& up, fract8& right, fract8& diag) // 3 Pointers to resulting fract8 variables
{
	fract8 x_fract = (hor >> 7) & 15;        // Extracts only the fractional part (in 1/16th) of the pixel position
	fract8 y_fract = (ver >> 7) & 15;

	diag = x_fract * y_fract;                 // Returns a number representing the proportion (in 1/256th) for each of the 4 pixels
	up = (16 - x_fract) * y_fract;  
	right = (16 - y_fract) * x_fract;
	pixel = 256 - (16 * x_fract) - (16 * y_fract) + diag;   
}

uint8_t gGravity = 7;
uint8_t  gDrag = 1;

accum88 gBurstx;
accum88 gBursty;
saccum78 gBurstxv;
saccum78 gBurstyv;
CRGB gBurstcolor;

#define NONE 0
#define SHELL 1
#define SPARK 2
#define EXPLODING 3
#define BURSTING 4

class Dot {
	public:
	int8_t	show;
	int8_t	theType;
	accum88 x;
	accum88 y;
	saccum78 xv;
	saccum78 yv;
	CRGB    color;

	Dot() {						// Constructor for the Dot class
		show = 0;
		theType = 0;
		x =  0;
		y =  0;
		xv = 0;
		yv = 0;
		color.setRGB( 0, 0, 0);
	}

	void Draw() {
		fract8 p00, p01, p10, p11;		// percentage of pixel spread to adjacent pixels
    
		screenscale( x, y, p00, p01, p10, p11);

		uint8_t x_pos = x >> 11;		// Scaling to get x pixel position 0 - 32
		x_pos = map(x_pos, 0, 32, 0, MATRIX_WIDTH-1);	// Scale to Matrix width

		uint8_t y_pos = y >> 10;		// Scaling to get y pixel position 0 - 64
		y_pos = map(y_pos, 0, 64, 0, MATRIX_HEIGHT-1);	// Scale to Matrix height

		if (yv > 0){				// In case of equal values, just adding 1 or 2 to any pixel's percentage
			if (p00 == p01) p01++;		// will ensure than only one pixel will have a higher percentage than  
			if (p10 == p11) p11++;		// any of the other 3
		}
		if (yv < 0){
			if (p00 == p01) p00++;
			if (p10 == p11) p10++;
		}
		if (xv > 0){
			if (p00 == p10) p10++;
			if (p01 == p11) p11++;
		}
		if (xv < 0){
			if (p00 == p10) p00++;
			if (p01 == p11) p01++;
		}
   
		if( !show) return;

		if ((p00 > p01) && (p00 > p10) && (p00 > p11)) setXY(x_pos, y_pos) += color;
		if ((p01 > p00) && (p01 > p10) && (p01 > p11)) setXY(x_pos, y_pos + 1) += color;
		if ((p10 > p00) && (p10 > p01) && (p10 > p11)) setXY(x_pos + 1, y_pos) += color;
		if ((p11 > p00) && (p11 > p01) && (p11 > p10)) setXY(x_pos + 1, y_pos + 1) += color;
		if ((p00 > p10) && (p01 > p10) && (p11 > p10)) setXY(x_pos - 1, y_pos) += color;
	    
		/*
		setXY(x_pos, y_pos) = CRGB(p00, p00, p00);		// Modifies the color content of the base pixel 
		setXY(x_pos, y_pos + 1) = CRGB(p01, p01, p01);		// And the 3 immediate pixels on top, to the right and diagonal
		setXY(x_pos + 1, y_pos) = CRGB(p10, p10, p10);
		setXY(x_pos + 1, y_pos + 1) = CRGB(p11, p11, p11);
		*/
    
	}                    // End of draw function

	void Move() {
//		if( !show) return;

		if(((xv >  0) && (x+xv < xv)) || ((xv < 0) && (x+xv > xv))) show = 0;	// Prevents pixels wraparounds from side to side
		if(((yv >  0) && (y+yv < yv)))  show = 0;				// Prevents pixels wraparounds from top to bottom
		if( yv < 0 && (y < (-yv)) ) show = 0;					// If velocity is negative AND we are about to hit the ground 

		if (theType == EXPLODING) {						// If the Shell has exploded...
	  
			show = 0;
			theType = BURSTING;						// Prepare for a burst on next frame 
//			gBurstx = x;                           
//			gBursty = y;
//			gBurstxv = xv;
//			gBurstyv = yv;
//			hsv2rgb_rainbow( CHSV( random8(), 240, 200), gBurstcolor);    
		} 
   
		if ((yv < 0) && (theType == SHELL)) {		// If velocity is negative AND it is a shell...
			xv=0;					// Stop further movements
			yv=0;
			color = CRGB(255, 255, 255);		// Set full brightness white color
			theType = EXPLODING;			// Explode the shell !! 
		}

		x += xv;
		y += yv;

		yv -= gGravity;
		if (xv>0) xv -= gDrag;
		if (xv<0) xv += gDrag;
		if (yv>0) yv -= gDrag;
		if (yv<0) yv += gDrag;

	}							// End of the move function 

	void GroundLaunch() {
		gGravity = map8(speed, 0, 6)+3;

		if (MATRIX_HEIGHT <= 32) yv = ((20*(MATRIX_HEIGHT+(3*(gGravity*0.8))))-(MATRIX_HEIGHT*5)) + random16(MATRIX_HEIGHT*5);	// Vertical velocity = Minimum velocity + Random maximum difference
		if (MATRIX_HEIGHT > 32)  yv = ((14*(MATRIX_HEIGHT+(3*(gGravity*0.8))))-(MATRIX_HEIGHT*5)) + random16(MATRIX_HEIGHT*3);	// Vertical velocity = Minimum velocity + Random maximum difference
		xv = random16(350) - 175;			// Generates a signed int value between +/- 175  (Nice width but always inside of frame)      
		y = 0;						// Ground launch
//		x = random16(); 				// Horizontal
		x = random16(0x8000)+0x2000;			// Horizontal middle 7/8 of the matrix
//		x = random16(0x4000)+0x2000;			// Horizontal middle 2/4 of the matrix
//		x = 0x4000;					// Horizontal middle of the matrix
		color.setRGB(24,24,24);				// Shells are white color' as a CRGB
		theType = SHELL;
		show = 1;
	}							// End of Groundlaunch function

	void Skyburst( accum88 basex, accum88 basey, CRGB& basecolor) {
		yv = random16(400) - 200;			// Random number (Y-velocity) between +/- 200
		xv = random16(400) - 200;			// Random number (X-velocity) between +/- 200
		y = basey;
		x = basex;
		color = basecolor;
		color *= 8;
		theType = SPARK;
		show = 1;
	}							// End of Skyburst function
};								// End of Dot class definition

#define MAX_SHELLS (MATRIX_HEIGHT/7)
#define MIN_SHELLS (MATRIX_HEIGHT/9)
#define MAX_SPARKS 40
#define MIN_SPARKS 20

Dot gDot[MAX_SHELLS];					//Creates an object named gDot of type Dot class
Dot gSparks[MAX_SHELLS][MAX_SPARKS];			//Creates an array object named gSparks of type Dot class

void fireworks() 
{
	CRGB sky1(0,0,17);				// Background sky color (will only work if brightness is set high 128 or up !!)
	CRGB sky2(32,32,64);				// Alternate sky color to create a star twinkle effect 

	for( uint8_t h = 0; h < MATRIX_WIDTH; h++) {	// All leds will be set to 'sky1' (very dark blue) 
		for( int v = 0; v < MATRIX_HEIGHT; v++) {
			matrixleds[XY2(h,v)] = sky1;
		}
	}

	if( random8() < 48 ) {
		matrixleds[XY2(random16(MATRIX_WIDTH),random16(MATRIX_HEIGHT))] = sky2;	// Around once each 8 frames, a random pixel is set to 'sky2' 
	}

	static uint8_t launchcountdown = 0;
	static uint16_t re_launchcountdown = 0;
     
	if(gDot[MAX_SHELLS-1].show == 0) {		// If the last shell has exploded
		re_launchcountdown--;			// Count down for next re_launch
		if (re_launchcountdown > 30000) {
			re_launchcountdown = 0;
		}
	}

	for( int a = 0; a < MAX_SHELLS; a++) {		// Always moves and draws the MAX number of sparks not the actual number of sparks (Wasteful!?)
		if(re_launchcountdown == 0) {
			if(gDot[a].show == 0) {
				if(launchcountdown == 0) {
					gDot[a].GroundLaunch();
					launchcountdown = random16(50) + 10;
				} else {
					launchcountdown--;
					if (launchcountdown > 254) {
						launchcountdown = 0;
					}
				}
			} 
		}

		if(gDot[MAX_SHELLS-1].show == 1) {
			//re_launchcountdown = random16(400) + 100;	// Last SHELL has launched, restart the relaunch timer
			re_launchcountdown = random16(100);	// Last SHELL has launched, restart the relaunch timer
		}

	//	if( gDot[a].theType == EXPLODING) {
	//		gDot[a].color.setRGB(255,255,255);
	//	}

		if( gDot[a].theType == BURSTING) {
			hsv2rgb_rainbow( CHSV( random8(), 255, random8(64,254)), gBurstcolor);
			gBurstx = gDot[a].x;
			gBursty = gDot[a].y;
			int nsparks = random8(MIN_SPARKS, MAX_SPARKS+1);
			for( int b = 0; b < nsparks; b++) {
				gSparks[a][b].Skyburst( gBurstx, gBursty, gBurstcolor);
			}
			gDot[a].theType = SPARK;
		}

		gDot[a].Move();					// Calculate the next position of the dots 
		gDot[a].Draw();					// Scale the position of the shell on the LED matrix 

		for( int b = 0; b < MAX_SPARKS; b++) {		// Always moves and draws the MAX number of sparks not the actual number of sparks (Wasteful!?)
			gSparks[a][b].Move();
			gSparks[a][b].color.r = gSparks[a][b].color.r * 255 /256;
			gSparks[a][b].color.g = gSparks[a][b].color.g * 255 /256;
			gSparks[a][b].color.b = gSparks[a][b].color.b * 255 /256;
			gSparks[a][b].Draw();
		}
	}
}

void setup() {
  delay( 1000 ); //safety startup delay
  Serial.begin(115200);
  matrix_setup();
  matrix->begin();
}

void loop()
{
  fireworks();
  
  matrix->show();
}

