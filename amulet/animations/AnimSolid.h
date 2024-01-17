#pragma once

#include "animation.h"

class AnimSolid : public Animation
{
public:
	CRGB rgb;
	virtual void init()
	{
		LOG_LV1("LED", "SolidHue::init");
		rgb.r = params_.color1_;
		rgb.g = params_.color2_;
		rgb.b = params_.extra0_;

		fill_solid(leds, RGB_LED_COUNT, rgb);
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		fill_solid(leds, RGB_LED_COUNT, rgb);
	}
};
