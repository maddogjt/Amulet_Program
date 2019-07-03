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
	animPattern pattern{};
	pattern.name = name;
	pattern.params.extra_[0] = p1;
	pattern.params.extra_[1] = p2;
	pattern.params.flags_ = 0;
	pattern.params.speed_ = 1;
	start_animation(pattern);
}

void start_animation(const animPattern &pattern)
{
	if (currentAnim != nullptr)
	{
		delete currentAnim;
		currentAnim = nullptr;
	}

	switch (pattern.name)
	{
#define DEFINE_ANIM(animName)             \
	case Anim::animName:                  \
		currentAnim = new animName();     \
		currentAnimName = Anim::animName; \
		break;

#include "AnimList.hpp"
#undef DEFINE_ANIM

	default:
		LOG_LV1("ANIM", "Bad animation name");
		break;
	}

	if (currentAnim != nullptr)
	{
		currentAnim->setParams(pattern.params);

		LOG_LV1("ANIM", "Current Anim changed to %d", currentAnimName);
		LOG_LV1("ANIM", "Param 0:	%d", pattern.params.extra_[0]);
		LOG_LV1("ANIM", "Param 1:	%d", pattern.params.extra_[1]);
		LOG_LV1("ANIM", "Param 2:	%d", pattern.params.extra_[2]);
		LOG_LV1("ANIM", "Param 3:	%d", pattern.params.extra_[3]);
		LOG_LV1("ANIM", "Fold: %d, Mirror: %d, Loop: %d",
				currentAnim->params_.flags_ & ANIMATION_FLAG_FOLD,
				currentAnim->params_.flags_ & ANIMATION_FLAG_MIRROR,
				currentAnim->params_.flags_ & ANIMATION_FLAG_LOOP);

		frame_counter = 0;
		currentAnim->init();
	}
}

void step_animation()
{
	if (currentAnim != nullptr)
	{
		currentAnim->step(frame_counter++, 0.f, 1.f);

		if (currentAnim->params_.flags_ & ANIMATION_FLAG_FOLD)
		{
			fold();
		}
		if (currentAnim->params_.flags_ & ANIMATION_FLAG_MIRROR)
		{
			mirror();
		}
		if (currentAnim->params_.flags_ & ANIMATION_FLAG_LOOP)
		{
			mirror_invert();
		}
	}
}

bool matches_current_animation(const animPattern &pattern)
{
	if (currentAnim == nullptr)
	{
		return false;
	}
	return currentAnimName == pattern.name &&
		   currentAnim->params_ == pattern.params;
}

bool matches_current_animation(Anim name, int p1, int p2)
{
	if (currentAnim == nullptr)
	{
		return false;
	}
	return currentAnimName == name &&
		   currentAnim->params_.extra_[0] == p1 &&
		   currentAnim->params_.extra_[1] == p2;
}

//
//  Functions below have not been converted to animations
//

void fold()
{
	/* *
	+------+
	\ 4  3 /
	| 5  2 |
	| 6  1 |
	/ 7  0 \
	+------+
	*/
	gLeds[2] = gLeds[1];
	gLeds[3] = gLeds[0];
	gLeds[4] = gLeds[7];
	gLeds[5] = gLeds[6];
}

void mirror_invert()
{
	for (int i = 0; i < 4; i++)
	{
		gLeds[7 - i] = CRGB(
			gLeds[i].g,
			gLeds[i].b,
			gLeds[i].r);
	}
}

void mirror()
{
	for (int i = 0; i < 4; i++)
	{
		gLeds[7 - i] = gLeds[i];
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
