#pragma once

#include "../Animation.h"

class AnimRainbow : public Animation
{
public:
	int hue;
	virtual void init()
	{
		LOG_LV1("LED", "Rainbow::init( %d, %d )", params_.extra_[0], params_.extra_[1]);
		hue = params_.color1_;
		fill_rainbow(leds, RGB_LED_COUNT, hue, 20);
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		hue += params_.speed_ / 16;
		fill_rainbow(leds, RGB_LED_COUNT, hue, 20);
	}
};
