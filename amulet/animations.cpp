#define DO_INCLUDES
#include "AnimList.hpp"

#include "animations.h"
#include "globals.h"
#include "AnimationMod.h"
#include "CSVHelpers.hpp"

#define NUM_LEDS RGB_LED_COUNT
CRGB gLeds[NUM_LEDS];

Animation *currentAnim = nullptr;
Anim currentAnimName = Anim::Unknown;

int frame_counter = 0;

const char *animNames_[]{
#define DEFINE_ANIM(name) #name,
#include "AnimList.hpp"
#undef DEFINE_ANIM
};

const char *get_animation_name(Anim anim)
{
	if ((int)anim >= 0 && anim < Anim::Count)
	{
		// offset by 4 to trim off "Anim" prefix
		return animNames_[(int)anim] + 4;
	}
	return "unknown";
}
int get_animations_count()
{
	return (int)Anim::Count;
}

void dump_animation_to_console(const animPattern &anim)
{
	Serial.printf("A: %d c1: %d c2: %d\n",
				  anim.name,
				  anim.params.color1_,
				  anim.params.color2_);
}

animPattern deserializeAnimPattern(char *str, uint8_t len)
{
	animPattern pattern;
	pattern.name = (Anim)first_tok(str);
	pattern.params.color1_ = next_tok();
	pattern.params.color2_ = next_tok();
	pattern.params.speed_ = next_tok();
	pattern.params.flags_ = next_tok();
	pattern.params.mask_ = next_tok();
	pattern.params.filter_ = next_tok();
	pattern.params.extra0_ = next_tok();
	pattern.params.extra1_ = next_tok();
	return pattern;
}

uint8_t serializeAnimPattern(char *buffer, const uint8_t len, const animPattern &pattern)
{
	memset(buffer, 0, len);
	append_int_comma(buffer, (int)pattern.name);
	append_int_comma(buffer, pattern.params.color1_);
	append_int_comma(buffer, pattern.params.color2_);
	append_int_comma(buffer, pattern.params.speed_);
	append_int_comma(buffer, pattern.params.flags_);
	append_int_comma(buffer, pattern.params.mask_);
	append_int_comma(buffer, pattern.params.filter_);
	append_int_comma(buffer, pattern.params.extra0_);
	append_int_comma(buffer, pattern.params.extra1_);
	buffer[strlen(buffer) - 1] = '\0'; // delete last comma
	Serial.printf("Anim Pattern (len: %d): %s\n", strlen(buffer), buffer);
	return strlen(buffer);
}

void start_animation(Anim name, int p1, int p2)
{
	animPattern pattern{};
	pattern.name = name;
	pattern.params.extra0_ = p1;
	pattern.params.extra1_ = p2;
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

		initMask(currentAnim->params_.mask_);

		LOG_LV1("ANIM", "Current Anim changed to %d", currentAnimName);
		LOG_LV1("ANIM", "Param 0:	%d", pattern.params.extra0_);
		LOG_LV1("ANIM", "Param 1:	%d", pattern.params.extra1_);
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
		for (int i = 0; i < RGB_LED_COUNT; i++)
		{
			gLeds[i] = currentAnim->leds[i];
		}

		float mask[RGB_LED_COUNT];
		getMask(mask, RGB_LED_COUNT, currentAnim->params_.mask_, frame_counter);
		filterLEDs(gLeds, mask, RGB_LED_COUNT, currentAnim->params_.filter_);

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
		   currentAnim->params_.extra0_ == p1 &&
		   currentAnim->params_.extra1_ == p2;
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

void addGlitter(fract8 chanceOfGlitter)
{
	if (random8() < chanceOfGlitter)
	{
		gLeds[random16(NUM_LEDS)] += CRGB::White;
	}
}

// void rainbowWithGlitter()
// {
// 	// built-in FastLED rainbow, plus some random sparkly glitter
// 	fill_rainbow(gLeds, 4, gHue++, 20);
// 	addGlitter(80);
// }
