#define DO_INCLUDES
#include "AnimList.hpp"

#include "animations.h"
#include "globals.h"


#define NUM_LEDS RGB_LED_COUNT
CRGB gLeds[NUM_LEDS];

uint8_t gHue = 0; // rotating "base color" used by many of the patterns

Animation *currentAnim = nullptr;
Anim currentAnimName = Anim::Unknown;

int frame_counter = 0;

void start_animation(Anim name, int p1, int p2)
{
	if (currentAnim != nullptr) {
		delete currentAnim;
		currentAnim = nullptr;
	}

	animParams params {};
	params.extra_[0] = p1;
	params.extra_[1] = p2;

	switch (name)
	{
	#define DEFINE_ANIM( animName ) case Anim::animName: \
		currentAnim = new animName(); \
		currentAnimName = Anim::animName; \
		break; 
	
	#include "AnimList.hpp"
	#undef DEFINE_ANIM
	
	default:
		LOG_LV1("ANIM", "Bad animation name");
		break;
	}


	if (currentAnim != nullptr) {
		currentAnim->setParams(params);

		  frame_counter = 0;
  			currentAnim->init();
	}
}

void step_animation( )
{
	if (currentAnim != nullptr) {
		currentAnim->step(frame_counter++, 0.f, 1.f);
	}
}


bool matches_current_animation(Anim name, int p1, int p2)
{
	if (currentAnim == nullptr) {
		return false;
	}
	return currentAnimName == name &&
		   currentAnim->params_.extra_[0] == p1 &&
		   currentAnim->params_.extra_[1] == p2;
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


