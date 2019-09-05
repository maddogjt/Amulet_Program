#pragma once

#include "animation.h"

class AnimRainbow : public Animation
{
public:
	float hue;
	virtual void init()
	{
		LOG_LV1("LED", "Rainbow::init");
		hue = params_.color1_;
		fill_rainbow(leds, RGB_LED_COUNT, hue, 20);
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		hue += params_.speed_ / 100.f;
		uint8_t iHue = (int)hue % 256;
		fill_rainbow(leds, RGB_LED_COUNT, iHue, 20);
	}
};
