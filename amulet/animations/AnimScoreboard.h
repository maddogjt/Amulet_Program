#pragma once

#include "animation.h"

class AnimScoreboard : public Animation
{
public:
	float hue;
	virtual void init()
	{
		LOG_LV1("LED", "Scoreboard::init");
		hue = params_.color1_;
		fill_solid(leds, RGB_LED_COUNT, CHSV(hue, 255, 192));
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		hue += params_.speed_ / 100.f;
		uint8_t iHue = (int)hue % 256;
		fill_solid(leds, RGB_LED_COUNT, CHSV(iHue, 255, 192));
	}
};
