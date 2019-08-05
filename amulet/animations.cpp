#define DO_INCLUDES
#include "AnimList.hpp"

#include "animations.h"
#include "globals.h"
#include "CSVHelpers.hpp"

#include "src/animation/animation_modifiers.h"
#include "src/animation/animation_overlay.h"

CRGB gLeds[RGB_LED_COUNT];

Animation *currentAnim = nullptr;
Anim currentAnimName = Anim::Unknown;

int frame_counter = 0;

constexpr const char *animNames_[]{
#define DEFINE_ANIM(name) #name,
#include "AnimList.hpp"
#undef DEFINE_ANIM
};

const char *animation_get_name(Anim anim)
{
	if ((int)anim >= 0 && anim < Anim::Count)
	{
		// offset by 4 to trim off "Anim" prefix
		return animNames_[(int)anim] + 4;
	}
	return "unknown";
}

void dump_animation_to_console(const anim_config_t &anim)
{
	Serial.printf("A: %d c1: %d c2: %d\n",
				  anim.name,
				  anim.params.color1_,
				  anim.params.color2_);
}

void start_animation(const anim_config_t &pattern)
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

		animation_overlay_set((Anim)currentAnim->params_.mask_, (OverlayFilter)currentAnim->params_.filter_);

		LOG_LV1("ANIM", "New Current Anim %s", animation_get_name(currentAnimName));
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
			animation_overlay_apply(gLeds, RGB_LED_COUNT, frame_counter, (int)(signalPower * 255));
		}
		else
		{
			animation_overlay_apply(gLeds, RGB_LED_COUNT, frame_counter);
		}

		AnimationModifier modifier = (AnimationModifier)(currentAnim->params_.flags_ & 0x0F);

		animation_modifier_apply(modifier, gLeds, RGB_LED_COUNT);
	}
	else
	{
		LOG_LV1("ERROR", "Current anim is null during step");
	}
}

bool matches_current_animation(const anim_config_t &pattern)
{
	if (currentAnim == nullptr)
	{
		return false;
	}
	return currentAnimName == pattern.name &&
		   currentAnim->params_ == pattern.params;
}
