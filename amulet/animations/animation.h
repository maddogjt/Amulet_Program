#pragma once

#include "../src/animation/animation_config.h"

#include <FastLED.h>

#define ANIMATION_FRAMERATE 120
#define ANIMATION_FLAG_USE_SIGNAL_POWER (0x10) /**< Set signal power parameter */

bool getCurrentAnimationPattern(anim_config_t &config);

class Animation
{
public:
	CRGB leds[RGB_LED_COUNT];

	virtual ~Animation() {}
	void setParams(const anim_config_t &config)
	{
		params_ = config;
	}
	virtual void init() {}
	virtual void step(const int frame, const float deltaTime, const float sourceDistance) = 0;

	anim_config_t params_;
};
