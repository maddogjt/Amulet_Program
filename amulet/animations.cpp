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

		LOG_LV1("ANIM", "New Current Anim %s", get_animation_name(currentAnimName));
		LOG_LV1("ANIM", "  F: %d M %d F: %d", currentAnim->params_.flags_, currentAnim->params_.mask_, currentAnim->params_.filter_);

		frame_counter = 0;
		currentAnim->init();
	}
}

void cycle_leds(int msPerCycle);
void blur_leds();
void flip_leds();
void scramble_leds();

void step_animation(Signal *topSignal)
{
	if (currentAnim != nullptr)
	{
		currentAnim->step(frame_counter++, 0.f, 1.f);
		for (int i = 0; i < RGB_LED_COUNT; i++)
		{
			gLeds[i] = currentAnim->leds[i];
		}

		if (currentAnim->params_.flags_ & ANIMATION_FLAG_USE_SIGNAL_POWER)
		{
			float signalPower = 1.f;
			if (currentAnim->params_.flags_ & ANIMATION_FLAG_USE_SIGNAL_POWER && topSignal != nullptr)
			{
				signalPower = normalizeRSSI(topSignal->_scan.rssi);
			}
			maskAndFilter(gLeds, RGB_LED_COUNT, currentAnim->params_.filter_, frame_counter, (int)(signalPower * 255));
		}
		else
		{
			maskAndFilter(gLeds, RGB_LED_COUNT, currentAnim->params_.filter_, frame_counter);
		}

		int effect = currentAnim->params_.flags_ & 0x0F;
		switch (effect)
		{
		case ANIMATION_EFFECT_FOLD:
			fold();
			break;
		case ANIMATION_EFFECT_MIRROR:
			mirror();
			break;
		case ANIMATION_EFFECT_LOOP:
			loop_leds();
			break;
		case ANIMATION_EFFECT_CYCLE:
			cycle_leds(500);
			break;
		case ANIMATION_EFFECT_SHIFT:
			mirror_invert();
			break;
		case ANIMATION_EFFECT_BLUR:
			blur_leds();
			break;
		case ANIMATION_EFFECT_FLIP:
			flip_leds();
			break;
		case ANIMATION_EFFECT_SCRAMBLE:
			scramble_leds();
			break;
		default:
			break;
		}
	}
	else
	{
		LOG_LV1("ERROR", "Current anim is null during step");
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

void swap(CRGB &c1, CRGB &c2)
{
	CRGB swap = c1;
	c1 = c2;
	c2 = swap;
}

void blur_leds()
{
	CRGB rows[RGB_LED_COUNT];
	rows[0] = gLeds[7];
	rows[1] = gLeds[0];
	rows[2] = gLeds[6];
	rows[3] = gLeds[1];
	rows[4] = gLeds[5];
	rows[5] = gLeds[2];
	rows[6] = gLeds[4];
	rows[7] = gLeds[3];
	blurRows(gLeds, 4, 2, 40);
	gLeds[7] = rows[0];
	gLeds[0] = rows[1];
	gLeds[6] = rows[2];
	gLeds[1] = rows[3];
	gLeds[5] = rows[4];
	gLeds[2] = rows[5];
	gLeds[4] = rows[6];
	gLeds[3] = rows[7];
}

void scramble_leds()
{
	CRGB scramble[RGB_LED_COUNT];
	scramble[0] = gLeds[3];
	scramble[1] = gLeds[1];
	scramble[2] = gLeds[0];
	scramble[3] = gLeds[6];
	scramble[4] = gLeds[5];
	scramble[5] = gLeds[4];
	scramble[6] = gLeds[7];
	scramble[7] = gLeds[2];
	for (int i = 0; i < RGB_LED_COUNT; i++)
	{
		gLeds[i] = scramble[i];
	}
}

void flip_leds()
{
	swap(gLeds[7], gLeds[4]);
	swap(gLeds[6], gLeds[5]);
}

void loop_leds()
{
	// bottom right => top left, top right => bottom left
	for (int i = 0; i < 4; i++)
	{
		gLeds[i + 4] = gLeds[i];
	}
}

int cycleStart = 0;
void cycle_leds(int msPerCycle)
{
	EVERY_N_MILLISECONDS(msPerCycle)
	{
		cycleStart = (cycleStart + 1) % RGB_LED_COUNT;
	}
	// Need a copy of gLeds so we don't overwrite before cycle;
	CRGB temp[RGB_LED_COUNT];
	for (int i = 0; i < RGB_LED_COUNT; i++)
	{
		temp[i] = gLeds[i];
	}

	for (int i = 0; i < RGB_LED_COUNT; i++)
	{
		gLeds[i] = temp[(i + cycleStart) % RGB_LED_COUNT];
	}
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
