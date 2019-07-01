#pragma once

#include "../Animation.h"

class AnimRainbow : public Animation
{
public:
	virtual void init()
	{
		LOG_LV1("LED", "Rainbow::init( %d, %d )", animation_p1, animation_p2);
		gHue = animation_p1;
		fill_rainbow(gLeds, RGB_LED_COUNT, gHue++, 20);
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		gHue += animation_p2;
		fill_rainbow(gLeds, RGB_LED_COUNT, gHue++, 20);
	}
};
