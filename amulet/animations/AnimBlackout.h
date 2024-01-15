#pragma once

#include "animation.h"

class AnimBlackout : public Animation
{
public:
	virtual void init()
	{
		LOG_LV1("LED", "Blackout::init");
		fill_solid(leds, RGB_LED_COUNT,CRGB(0,0,0));
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		fill_solid(leds, RGB_LED_COUNT, CRGB(0, 0, 0));
	}
};
