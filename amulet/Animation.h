#pragma once

#include <cstdint>
#include <Arduino.h>
#include "globals.h"
#define ARDUINO_GENERIC
#include <FastLED.h>
#undef ARDUINO_GENERIC
FASTLED_USING_NAMESPACE

struct animParams
{
	uint8_t color1_;
	uint8_t color2_;
	uint8_t speed_;
	uint8_t flags_;
	uint8_t mods_;
	uint8_t extra0_;
	uint8_t extra1_;
	bool operator==(const animParams &p) const
	{
		return color1_ == p.color1_ && color2_ == p.color2_ && mods_ == p.mods_ &&
			   speed_ == p.speed_ && flags_ == p.flags_ && extra0_ == p.extra0_ && extra1_ == p.extra1_;
	};
};

extern CRGB gLeds[RGB_LED_COUNT];

#define ANIMATION_FLAG_FOLD (0x01)			   /**< Top 4 leds mirror bottom 4 leds */
#define ANIMATION_FLAG_MIRROR (0x02)		   /**< The left side mirrors the right side */
#define ANIMATION_FLAG_LOOP (0x04)			   /**< The left side mirrors the right side, but upside down */
#define ANIMATION_FLAG_USE_SIGNAL_POWER (0x08) /**< Set signal power parameter */
#define ANIMATION_FLAG_NO_GRADIENT (0x10)	  /**< Just use the defined colors */
#define ANIMATION_FLAG_FULL_SPECTRUM (0x20)	/**< Use full spectrum instead of a gradient */

void mirror();

class Animation
{
public:
	CRGB leds[RGB_LED_COUNT];

	virtual ~Animation() {}
	void setParams(const animParams &parameters)
	{
		params_ = parameters;
	}
	virtual void init() {}
	virtual void step(const int frame, const float deltaTime, const float sourceDistance) = 0;

	animParams params_;
};
