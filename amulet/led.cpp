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
bool led_test_mode = false;

animation_t current_animation;

// returns true if animation was changed
bool set_animation_from_signal( Signal *s) {
  animation_t anim_from_signal;
  if( s && (s->_scan._cmd == command_beacon || s->_scan._cmd == command_flashmob ) ) {
    animation_for_name( anim_from_signal, (animation_name_type_t) s->_scan._p0, s->_scan._p1, s->_scan._p2);
  } else {
    show_ambient = true;
    animation_for_name( anim_from_signal, ANIMATION_SOLID_HUE, 0, 1);
  }
  if( !animations_are_equal(current_animation, anim_from_signal) ) {
    current_animation = anim_from_signal;
    return true;
  } else {
    return false;
  }
}

void led_setup()
{
    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(gLeds, RGB_LED_COUNT).setCorrection(TypicalLEDStrip);

    // set master brightness control
    FastLED.setBrightness(BRIGHTNESS);

    // Set the initial animation
    animation_for_name( current_animation, ANIMATION_SOLID_HUE, 0, 1);
    start_animation(current_animation);
}

void choose_pattern_by_signal() {
  LOG_LV1("LED", "choose_pattern_by_signal");
  Signal *signal = current_top_signal();
  LOG_LV1("LED", "Has Top Signal: %s", signal ? "Yes" : "No");
  if( set_animation_from_signal(signal) ) {
    start_animation(current_animation);
  }
}

void led_loop( int step )
{
  // Update the LED pattern based on bluetooth signals every 500ms
  if( !led_test_mode && step % 12 == 0 ) { 
    choose_pattern_by_signal(); 
  }

  // Step the currentAnimation
  step_animation(current_animation);
  FastLED.show();
}

// -----------------------
//
// TEST PATTERN CODE
//
// -----------------------
int test_pattern_idx=0;
const int test_pattern_count = 5;

void led_show_test_pattern_by_index(int idx)
{
  switch (idx) 
  {
  case 0:
    animation_for_name( current_animation, ANIMATION_SOLID_HUE, 0, 5);
    break;
  case 1:
    animation_for_name( current_animation, ANIMATION_RAINBOW, 0, 2);
    break;
  case 2:
    animation_for_name( current_animation, ANIMATION_TWISTER, 0, 0);
    break;
  case 3:
    animation_for_name( current_animation, ANIMATION_FLAME, 0, 0);
    break;
  case 4:
    animation_for_name( current_animation, ANIMATION_DEBUG_INFO, 5, 0);
    break;
  default:
    break;
  }
  start_animation(current_animation);
  led_test_mode = true;
}

void led_show_test_pattern(bool show)
{
  led_test_mode = show;
  if( led_test_mode ) {
    test_pattern_idx = 0;
    led_show_test_pattern_by_index( 0 ); 
  }
}

bool led_test_has_next_pattern()
{
  return test_pattern_idx + 1 < test_pattern_count;
}

void led_show_next_test_pattern()
{
  test_pattern_idx = (test_pattern_idx + 1) % test_pattern_count;
  led_show_test_pattern_by_index( test_pattern_idx ); 
}

