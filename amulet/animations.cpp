
#include "animations.h"
#include "globals.h"

FASTLED_USING_NAMESPACE

#define NUM_LEDS RGB_LED_COUNT
CRGB gLeds[NUM_LEDS];

uint8_t gHue = 0;                  // rotating "base color" used by many of the patterns

// Rainbow: Each LED has a progressive hue and rotates by incrementing base hue
// p1 : Start hue
// p2 : rate of hue change per frame ( can be positive or negative )
void Rainbow::init(int p1, int p2) {
  LOG_LV1("LED", "Rainbow::init( %d, %d )", p1, p2);
  gHue = p1;
  _p2 = p2;
  fill_rainbow(gLeds, NUM_LEDS, gHue++, 20);
}
void Rainbow::step( int frame ) {
  gHue += _p2;
}

// Twister: randomly rotates through the colors of th twister mat
// no parameters
CRGB g_twister_colors[] = {
    CRGB::Green,
    CRGB::Yellow,
    CRGB::Blue,
    CRGB::Red
};
void Twister::init(int p1, int p2) {
  LOG_LV1("LED", "Twister::init( %d, %d )", p1, p2);
  for( int i =0; i<NUM_LEDS; i++) {
    gLeds[i] = g_twister_colors[random8() % ARRAY_SIZE(g_twister_colors)];
  }
}
void Twister::step( int frame ) {
  if( frame % 24 == 0 ) {
    for( int i =0; i<NUM_LEDS; i++) {
      if( random8() % 3 == 0 ) {
        gLeds[i] = g_twister_colors[random8() % ARRAY_SIZE(g_twister_colors)];
      }
    }
  }
}

// SolidHue: All LEDs are the same color, and can optionally rotate hue 
// p1 : Start hue
// p2 : rate of hue change per frame ( can be 0, positive, or negative )
void SolidHue::init(int p1, int p2) {
  LOG_LV1("LED", "SolidHue::init( %d, %d )", p1, p2);
  _p2 = p2;
  gHue = p1;
  fill_solid(gLeds, NUM_LEDS, CHSV(gHue, 255, 192));
}
void SolidHue::step( int frame ) {
  gHue += _p2;
  fill_solid(gLeds, NUM_LEDS, CHSV(gHue, 255, 192));
}



//
//  Functions below have not been converted to Animation subclasses
// 


void mirror()
{
  for (int i = 0; i < 4; i++) {
    gLeds[7-i] = gLeds[i];
  }
}

void rainbow2()
{
  // FastLED's built-in rainbow generator
  fill_rainbow(gLeds, 4, gHue++, 20);
  mirror();
}

void cylon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(gLeds, 4, 20);
  int pos = beatsin16(26, 0, 4 - 1);
  gLeds[pos] += CHSV(gHue, 255, 192);
  mirror();
}

void addGlitter(fract8 chanceOfGlitter)
{
  if (random8() < chanceOfGlitter)
  {
    gLeds[random16(NUM_LEDS)] += CRGB::White;
  }
}

void rainbowWithGlitter()
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  fill_rainbow(gLeds, 4, gHue++, 20);
  addGlitter(80);
}
void confetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy(gLeds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  gLeds[pos] += CHSV(gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(gLeds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS - 1);
  gLeds[pos] += CHSV(gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
  for (int i = 0; i < NUM_LEDS; i++)
  { //9948
    gLeds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
}

void juggle()
{
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy(gLeds, NUM_LEDS, 20);
  byte dothue = 0;
  for (int i = 0; i < 8; i++)
  {
    gLeds[beatsin16(i + 7, 0, NUM_LEDS - 1)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}
