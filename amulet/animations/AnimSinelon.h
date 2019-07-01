#pragma once

#include "../Animation.h"

class AnimSinelon : public Animation
{
public:
	virtual void init()
	{
		LOG_LV1("LED", "Sinelon::init( %d, %d )", animation_p1, animation_p2);
		gHue = animation_p1;
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		// a colored dot sweeping back and forth, with fading trails
		fadeToBlackBy(gLeds, RGB_LED_COUNT, 20);
		int pos = beatsin16(13, 0, RGB_LED_COUNT - 1);
		gLeds[pos] += CHSV(gHue, 255, 192);
		gHue += animation_p2;
	}
};
