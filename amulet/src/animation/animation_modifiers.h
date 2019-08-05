#pragma once

#include <cstdint>
#include <FastLED.h>

enum class AnimationModifier : uint8_t{
	None,
	Fold,   /**< Top 4 leds mirror bottom 4 leds */
	Mirror, /**< The left side mirrors the right side */
	Loop,   /**< The left side mirrors the right side, but upside down */
	Cycle,
	Shift,
	Blur,
	Flip,
	Scramble,

	Count
};

const char *animation_modifier_get_name(AnimationModifier modifier);
void animation_modifier_apply(AnimationModifier modifier, CRGB leds[], size_t count);