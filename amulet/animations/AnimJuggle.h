#pragma once

#include "../Animation.h"

class AnimJuggle : public Animation
{
public:
	virtual void init()
	{
		LOG_LV1("LED", "Juggle::init( %d, %d )", params_.extra0_, params_.extra1_);
		fill_solid(leds, RGB_LED_COUNT, CHSV(params_.color1_, 255, 0));
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		// eight colored dots, weaving in and out of sync with each other
		fadeToBlackBy(leds, RGB_LED_COUNT, 20);
		byte dothue = 0;
		for (int i = 0; i < 8; i++)
		{
			leds[beatsin16(i + 7, 0, RGB_LED_COUNT - 1)] |= CHSV(dothue, 200, 255);
			dothue += 32;
		}
	}
};
