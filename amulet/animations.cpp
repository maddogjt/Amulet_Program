
#include "animations.h"
#include "globals.h"

FASTLED_USING_NAMESPACE

#define NUM_LEDS RGB_LED_COUNT
CRGB gLeds[NUM_LEDS];

uint8_t gHue = 0; // rotating "base color" used by many of the patterns
int animation_p1=0;
int animation_p2=0;

int frame_counter = 0;
void start_animation( animation_t &anim)
{
  frame_counter = 0;
  animation_p1 = anim.p1;
  animation_p2 = anim.p2;
  anim.init();
}

void step_animation( animation_t &anim)
{
  anim.step(frame_counter++);
}

// Rainbow: Each LED has a progressive hue and rotates by incrementing base hue
// p1 : Start hue
// p2 : rate of hue change per frame ( can be positive or negative )
void Rainbow_init() {
  LOG_LV1("LED", "Rainbow::init( %d, %d )", animation_p1, animation_p2);
  gHue = animation_p1;
  fill_rainbow(gLeds, NUM_LEDS, gHue++, 20);
}
void Rainbow_step( int frame ) {
  gHue += animation_p2;
  fill_rainbow(gLeds, NUM_LEDS, gHue++, 20);
}

// Twister: randomly rotates through the colors of th twister mat
// no parameters
CRGB g_twister_colors[] = {
    CRGB::Green,
    CRGB::Yellow,
    CRGB::Blue,
    CRGB::Red
};
void Twister_init() {
  LOG_LV1("LED", "Twister::init( %d, %d )", animation_p1, animation_p2);

  for( int i =0; i<NUM_LEDS; i++) {
    gLeds[i] = g_twister_colors[random8() % ARRAY_SIZE(g_twister_colors)];
  }
}
void Twister_step( int frame ) {
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
void SolidHue_init() {
  LOG_LV1("LED", "SolidHue::init( %d, %d )", animation_p1, animation_p2);
  gHue = animation_p1;
  fill_solid(gLeds, NUM_LEDS, CHSV(gHue, 255, 192));
}
void SolidHue_step( int frame ) {
  gHue += animation_p2;
  fill_solid(gLeds, NUM_LEDS, CHSV(gHue, 255, 192));
}

// Flame: Adapted from https://github.com/FastLED/FastLED/blob/master/examples/Fire2012WithPalette/Fire2012WithPalette.ino
// p1 : 
// p2 : 
bool gReverseDirection = false;
CRGBPalette16 gPal;
void Flame_init() {
  LOG_LV1("LED", "Flame::init( %d, %d )", animation_p1, animation_p2);
  gPal = HeatColors_p;
  fill_solid(gLeds, NUM_LEDS, CHSV(gHue, 255, 192));
}
void Fire2012WithPalette();
void Flame_step( int frame ) {
  // Add entropy to random number generator; we use a lot of it.
  random16_add_entropy( random());
  Fire2012WithPalette(); // run simulation frame, using palette colors
  mirror();
}
#define COOLING  30
#define SPARKING 180
void Fire2012WithPalette()
{
// Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS; j++) {
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8( heat[j], 240);
      CRGB color = ColorFromPalette( gPal, colorindex);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (NUM_LEDS-1) - j;
      } else {
        pixelnumber = j;
      }
      gLeds[pixelnumber] = color;
    }
}

// DebugInfo: Lights LEDs as parameters to provide debug info. Each number is between 1-8.
// Zero is not allowed since you can't see zero lights.
// p1 : The first int 
// p2 : Hue
void DebugInfo_init() {
  LOG_LV1("LED", "DebugInfo::init( %d, %d )", animation_p1, animation_p2);
  gHue = animation_p2;
  fill_solid(gLeds, NUM_LEDS, CHSV(gHue, 255, 0));
  fill_solid(gLeds, max(1,min(NUM_LEDS,animation_p1)), CHSV(gHue, 255, 64));
}
void DebugInfo_step( int frame ) {
  // Pulse the brightness between 64 and 192
  fill_solid(gLeds, max(1,min(NUM_LEDS,animation_p1)), CHSV(gHue, 255, 64 + sin8(frame)/2));
}

// Cylon: 
// p1 : start hue
// p2 : hue increment
void Cylon_init() {
  LOG_LV1("LED", "Cylon::init( %d, %d )", animation_p1, animation_p2);
  gHue = animation_p1;
  fill_solid(gLeds, NUM_LEDS, CHSV(gHue, 255, 0));
}
void Cylon_step( int frame ) {
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(gLeds, 4, 20);
  int pos = beatsin16(26, 0, 4 - 1);
  gLeds[pos] += CHSV(gHue, 255, 192);
  mirror();
  gHue += animation_p2;
}

// Juggle: 
// p1 : 
// p2 : 
void Juggle_init() {
  LOG_LV1("LED", "Juggle::init( %d, %d )", animation_p1, animation_p2);
  gHue = animation_p1;
  fill_solid(gLeds, NUM_LEDS, CHSV(gHue, 255, 0));
}
void Juggle_step( int frame )
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

// Sinelon: 
// p1 : Start Hue
// p2 : Hue increment
void Sinelon_init() {
  LOG_LV1("LED", "Juggle::init( %d, %d )", animation_p1, animation_p2);
  gHue = animation_p1;
}
void Sinelon_step( int frame )
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(gLeds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS - 1);
  gLeds[pos] += CHSV(gHue, 255, 192);
  gHue += animation_p2;
}


//
//  Functions below have not been converted to animations
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


