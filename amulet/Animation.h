#pragma once

#include <cstdint>
#include <Arduino.h>
#include "globals.h"
#include <FastLED.h>
FASTLED_USING_NAMESPACE

#include "src/animation/animation_config.h"


#define ANIMATION_FLAG_USE_SIGNAL_POWER (0x10) /**< Set signal power parameter */
#define ANIMATION_FLAG_NO_GRADIENT (0x20)	  /**< Just use the defined colors */
#define ANIMATION_FLAG_FULL_SPECTRUM (0x40)	/**< Use full spectrum instead of a gradient */

void mirror();

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
