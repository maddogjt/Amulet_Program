
#include <FastLED.h>
#include "Animation.h"

FASTLED_USING_NAMESPACE

extern CRGB gLeds[RGB_LED_COUNT];

// class Rainbow : public Animation {
//     void init();
//     void step(int step);
// };
ANIMATION_SUBCLASS_DECL(SolidHue)
ANIMATION_SUBCLASS_DECL(Twister)
ANIMATION_SUBCLASS_DECL(Rainbow)
// class SolidHue : public Animation {
//     public:
//     void init(int p1, int p2);
//     void step(int step);
// };

// ANIMATION_SUBCLASS_DECL(Twister)
// ANIMATION_SUBCLASS_DECL(SolidHue)
// class Twister : public Animation {};
// class SolidHue : public Animation {};

void rainbow2();
void cylon();
void confetti();
void sinelon();
