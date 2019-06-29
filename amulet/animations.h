
#include <FastLED.h>
#include "Animation.h"

FASTLED_USING_NAMESPACE

extern CRGB gLeds[RGB_LED_COUNT];
extern int animation_p1;
extern int animation_p2;

void start_animation( animation_t &anim);
void step_animation( animation_t &anim);


void Rainbow_init();
void Rainbow_step( int frame );

void SolidHue_init();
void SolidHue_step( int frame );

void Twister_init();
void Twister_step( int frame );

void DebugInfo_init();
void DebugInfo_step( int frame );

void Flame_init();
void Flame_step( int frame );


// Functions that should be adapted to animations or removed
void rainbow2();
void cylon();
void confetti();
void sinelon();
void mirror();

