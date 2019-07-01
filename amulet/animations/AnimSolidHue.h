#pragma once

#include "../Animation.h"

class AnimSolidHue : public Animation
{
public:
	virtual void init()
	{
		LOG_LV1("LED", "SolidHue::init( %d, %d )", animation_p1, animation_p2);
		gHue = animation_p1;
		fill_solid(gLeds, RGB_LED_COUNT, CHSV(gHue, 255, 192));
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		gHue += animation_p2;
		fill_solid(gLeds, RGB_LED_COUNT, CHSV(gHue, 255, 192));
	}
};
