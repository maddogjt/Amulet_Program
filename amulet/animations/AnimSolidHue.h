#pragma once

#include "../Animation.h"

class AnimSolidHue : public Animation
{
public:
	int hue;
	virtual void init()
	{
		LOG_LV1("LED", "SolidHue::init( %d, %d )", params_.extra0_, params_.extra1_);
		hue = params_.color1_;
		fill_solid(leds, RGB_LED_COUNT, CHSV(hue, 255, 192));
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		hue += params_.speed_ / 16;
		fill_solid(leds, RGB_LED_COUNT, CHSV(hue, 255, 192));
	}
};
