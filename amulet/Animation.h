#pragma once

#include <cstdint>
#include <Arduino.h>
#include "globals.h"
#include <FastLED.h>
FASTLED_USING_NAMESPACE

struct animColor
{
	uint8_t h;
	// uint8_t s;
	// uint8_t v;
	bool operator==(const animColor &c) const
	{
		return h == c.h; // && s == c.s && v == c.v;
	};
};

#define ANIM_EXTRAS_SIZE 2
struct animParams
{
	animColor color1_;
	animColor color2_;
	animColor color3_;
	uint8_t speed_;
	uint8_t flags_;
	uint8_t extra_[ANIM_EXTRAS_SIZE];
	bool operator==(const animParams &p) const
	{
		bool extrasMatch = !memcmp(extra_, p.extra_, ANIM_EXTRAS_SIZE);
		return color1_ == p.color1_ && color2_ == p.color2_ && color3_ == p.color3_ &&
			   speed_ == p.speed_ && flags_ == p.flags_ && extrasMatch;
	};
};

// TODO: remove?
extern uint8_t gHue;
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
	virtual ~Animation() {}
	void setParams(const animParams &parameters)
	{
		params_ = parameters;
	}
	virtual void init() {}
	virtual void step(const int frame, const float deltaTime, const float sourceDistance) = 0;

	animParams params_;
};
