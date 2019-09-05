#pragma once

#include "animation.h"

class AnimSinelon : public Animation
{
public:
	int speed;
	int hue;
	virtual void init()
	{
		LOG_LV1("LED", "Sinelon::init");
		hue = params_.color1_;
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		// a colored dot sweeping back and forth, with fading trails
		fadeToBlackBy(leds, RGB_LED_COUNT, 20);
		int pos = beatsin16(params_.speed_, 0, RGB_LED_COUNT - 1);
		leds[pos] += CHSV(hue, 255, 192);
		hue += params_.extra0_;
	}
};
