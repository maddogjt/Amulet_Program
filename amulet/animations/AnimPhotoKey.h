#pragma once

#include "animation.h"

class AnimPhotoKey : public Animation
{
public:
	virtual void init()
	{
		LOG_LV1("LED", "PhotoKey::init");
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		fadeToBlackBy(leds, RGB_LED_COUNT, 10);
		int pos = random16(RGB_LED_COUNT);
		leds[pos] += CHSV(params_.color1_ + random8(64), 200, 255);
	}
};
