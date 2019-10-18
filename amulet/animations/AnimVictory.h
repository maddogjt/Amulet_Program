#pragma once

#include "animation.h"

class AnimVictory : public Animation
{
public:
	int hue;
	int hue2;
	virtual void init()
	{
		LOG_LV1("LED", "Victory::init");
		hue = params_.color1_;
		hue2 = params_.color2_;
		fill_solid(leds, RGB_LED_COUNT, CHSV(hue, 255, 0));
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		EVERY_N_MILLISECONDS(100)
		{
			fadeToBlackBy(leds, RGB_LED_COUNT, 110);
			leds[random8(0, 4)] = CHSV((hue2 += 30), 255, 192);
			leds[random8(4, 8)] = CHSV((hue2 += 30), 255, 192);
		}
	}
};
