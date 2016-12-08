#include "FastLED.h"

#define LED_PIN     3
#define LED_TYPE    WS2811
#define COLOR_ORDER RGB
#define NUM_LEDS    50
CRGB leds[NUM_LEDS];

// Original 'Twinkling Holiday Lights' by Mark Kriegsman, December 2014
// Modified to accept ColorPallette change via Serial Port.
 
#define MASTER_BRIGHTNESS   96

#define STARTING_BRIGHTNESS 64
#define FADE_IN_SPEED       20
#define FADE_OUT_SPEED      10
#define DENSITY            255

String inputString = "";

void setup() {
  delay(3000);
  FastLED.addLeds<LED_TYPE,LED_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(MASTER_BRIGHTNESS);
  Serial.begin(9600);
  inputString.reserve(5); // reserve 5 bytes
  chooseColorPalette('w');
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    chooseColorPalette(inChar);
  }
}

void loop()
{
  colortwinkles();
  FastLED.show();  
  FastLED.delay(40);
}


CRGBPalette16 gPalette;

void chooseColorPalette(char choose)
{
  uint8_t numberOfPalettes = 5;
  uint8_t secondsPerPalette = 10;

  CRGB r(CRGB::Red), b(CRGB::Blue), w(85,85,85), g(CRGB::Green), W(CRGB::White), l(0xE1A024), o(CRGB::Orange);

  switch(choose) {  
    case 'w': // Red, Green, and White
      gPalette = CRGBPalette16( r,r,r,r, g,g,g,g, W,W,W,W, r,r,r,r ); 
      break;
    case 'r': // Red
      gPalette = CRGBPalette16( r,r,r,r, r,r,r,r, r,r,r,r, r,r,r,r ); 
      break;
    case 'o': // Orange
      gPalette = CRGBPalette16( o,o,o,o, o,o,o,o, o,o,o,o, o,o,o,o ); 
      break;
    case 'b': // blue
      gPalette = CRGBPalette16( b,b,b,b, b,b,b,b, b,b,b,b, b,b,b,b );
      break;
    case 'g': // green
      gPalette = CRGBPalette16( g,g,g,g, g,g,g,g, g,g,g,g, g,g,g,g );
      break;
  }
}

enum { GETTING_DARKER = 0, GETTING_BRIGHTER = 1 };

void colortwinkles()
{
  // Make each pixel brighter or darker, depending on
  // its 'direction' flag.
  brightenOrDarkenEachPixel( FADE_IN_SPEED, FADE_OUT_SPEED);
  
  // Now consider adding a new random twinkle
  if( random8() < DENSITY ) {
    int pos = random16(NUM_LEDS);
    if( !leds[pos]) {
      leds[pos] = ColorFromPalette( gPalette, random8(), STARTING_BRIGHTNESS, NOBLEND);
      setPixelDirection(pos, GETTING_BRIGHTER);
    }
  }
}

void brightenOrDarkenEachPixel( fract8 fadeUpAmount, fract8 fadeDownAmount)
{
 for( uint16_t i = 0; i < NUM_LEDS; i++) {
    if( getPixelDirection(i) == GETTING_DARKER) {
      // This pixel is getting darker
      leds[i] = makeDarker( leds[i], fadeDownAmount);
    } else {
      // This pixel is getting brighter
      leds[i] = makeBrighter( leds[i], fadeUpAmount);
      // now check to see if we've maxxed out the brightness
      if( leds[i].r == 255 || leds[i].g == 255 || leds[i].b == 255) {
        // if so, turn around and start getting darker
        setPixelDirection(i, GETTING_DARKER);
      }
    }
  }
}

CRGB makeBrighter( const CRGB& color, fract8 howMuchBrighter) 
{
  CRGB incrementalColor = color;
  incrementalColor.nscale8( howMuchBrighter);
  return color + incrementalColor;
}

CRGB makeDarker( const CRGB& color, fract8 howMuchDarker) 
{
  CRGB newcolor = color;
  newcolor.nscale8( 255 - howMuchDarker);
  return newcolor;
}


// For illustration purposes, there are two separate implementations
// provided here for the array of 'directionFlags': 
// - a simple one, which uses one byte (8 bits) of RAM for each pixel, and
// - a compact one, which uses just one BIT of RAM for each pixel.

// Set this to 1 or 8 to select which implementation
// of directionFlags is used.  1=more compact, 8=simpler.
#define BITS_PER_DIRECTION_FLAG 1


#if BITS_PER_DIRECTION_FLAG == 8
// Simple implementation of the directionFlags array,
// which takes up one byte (eight bits) per pixel.
uint8_t directionFlags[NUM_LEDS];

bool getPixelDirection( uint16_t i) {
  return directionFlags[i];
}

void setPixelDirection( uint16_t i, bool dir) {
  directionFlags[i] = dir;
}
#endif


#if BITS_PER_DIRECTION_FLAG == 1
// Compact (but more complicated) implementation of
// the directionFlags array, using just one BIT of RAM
// per pixel.  This requires a bunch of bit wrangling,
// but conserves precious RAM.  The cost is a few
// cycles and about 100 bytes of flash program memory.
uint8_t  directionFlags[ (NUM_LEDS+7) / 8];

bool getPixelDirection( uint16_t i) {
  uint16_t index = i / 8;
  uint8_t  bitNum = i & 0x07;
  // using Arduino 'bitRead' function; expanded code below
  return bitRead( directionFlags[index], bitNum);
  // uint8_t  andMask = 1 << bitNum;
  // return (directionFlags[index] & andMask) != 0;
}

void setPixelDirection( uint16_t i, bool dir) {
  uint16_t index = i / 8;
  uint8_t  bitNum = i & 0x07;
  // using Arduino 'bitWrite' function; expanded code below
  bitWrite( directionFlags[index], bitNum, dir);
  //  uint8_t  orMask = 1 << bitNum;
  //  uint8_t andMask = 255 - orMask;
  //  uint8_t value = directionFlags[index] & andMask;
  //  if( dir ) {
  //    value += orMask;
  //  }
  //  directionFlags[index] = value;
}
#endif

