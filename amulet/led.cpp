#include <FastLED.h>
#include "Animation.h"
#include "animations.h"
#include "signal.h"
#include "globals.h"

FASTLED_USING_NAMESPACE

#define DATA_PIN PIN_RGB_LEDS
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define BRIGHTNESS 8

bool show_ambient= true;

Animation *currentAnimation;
SolidHue anim_solidHue;
Twister anim_twister;
Rainbow anim_rainbow;

// List of patterns to cycle through.  Each is defined as a separate function below.
const int anim_count = 3;
void setCurrentAnimationForPatternIdx( int pattern_idx ) {
  int idx = max(0, min(anim_count-1, pattern_idx));
  switch (idx) {
    case 0:
      currentAnimation = &anim_solidHue;
      break;
    case 1:
      currentAnimation = &anim_rainbow;
      break;
    case 2:
      currentAnimation = &anim_twister;
      break;
    default:
      LOG_LV1("LED", "Bad animation index");
  }
}

// Patterns are animation templates
typedef struct {
  int function_idx;
  int param1;
  int param2;
} pattern_t;

pattern_t pattern_from_signal( Signal *s) {
  if( s && (s->_scan._cmd == command_beacon || s->_scan._cmd == command_flashmob ) ) {
    return {
      .function_idx = max(0,min(anim_count,s->_scan._p0)),
      .param1 = s->_scan._p1,
      .param2 = s->_scan._p2,
    };
  }
  else {
    return {
      .function_idx = 0, // solidHue
      .param1 = 0,
      .param2 = 1,
    };
  }
}

void printPattern( pattern_t &p) {
  Serial.println("Pattern:");
  Serial.printf("func idx: %d\n", p.function_idx);
  Serial.printf("param 1:  %d\n", p.param1);
  Serial.printf("param 2:  %d\n\n", p.param2);
}

bool patternsAreEqual( pattern_t &p1, pattern_t &p2) {
  return p1.function_idx == p2.function_idx && p1.param1 == p2.param1 && p1.param2 == p2.param2;
}


pattern_t currentPattern = {
  .function_idx = 0,
  .param1 = 0,
  .param2 = 1,
};

void led_setup()
{
    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(gLeds, RGB_LED_COUNT).setCorrection(TypicalLEDStrip);

    // set master brightness control
    FastLED.setBrightness(BRIGHTNESS);

    // Set the initial animation
    currentAnimation = &anim_solidHue;
    currentAnimation->init(0,1);
}

void choose_pattern_by_signal() {
  LOG_LV1("LED", "choose_pattern_by_signal");

  Signal *signal = current_top_signal();
  LOG_LV1("LED", "Has Top Signal: %s", signal ? "Yes" : "No");
  pattern_t pattern = pattern_from_signal( signal );
  if( !patternsAreEqual( pattern, currentPattern )) {
    LOG_LV1("LED", "New Pattern Chosen");
    printPattern(pattern);
    currentPattern = pattern;
    setCurrentAnimationForPatternIdx(pattern.function_idx);
    currentAnimation->init(pattern.param1,pattern.param2);
  }
}

void led_loop( int step )
{
  // Update the LED pattern based on bluetooth signals every 500ms
  if( step % 12 == 0 ) { 
    choose_pattern_by_signal(); 
  }

  // Step the currentAnimation
  currentAnimation->step( step );
  FastLED.show();
}

