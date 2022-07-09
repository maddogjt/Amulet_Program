#pragma once

#include <FastLED.h>

enum class AnimationModifier : uint8_t{
	None,
	Fold,   /**< Top 4 leds mirror bottom 4 leds */
	Mirror, /**< The left side mirrors the right side */
	Loop,   /**< The left side mirrors the right side, but upside down */
	Cycle,  /**< Slowly moves the pattern around the amulet */
	Shift,  /**< mirror, but swaps the RGB channels on one half  */
	Blur, 	/**< uses fastled blur func. not sure if its done right  */
	Flip,	/**< Just swaps the middle leds  */
	Scramble,	/**< Swaps the LEDs in a fixed but arbitrary way  */

	Count
};

const char *animation_modifier_get_name(AnimationModifier modifier);
void animation_modifier_apply(AnimationModifier modifier, CRGB leds[], size_t count);