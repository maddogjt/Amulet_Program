#pragma once

#include "../Animation.h"

class AnimSinelon : public Animation
{
public:
	int speed;
	int hue;
	virtual void init()
	{
		LOG_LV1("LED", "Sinelon::init( %d, %d )", params_.extra_[0], params_.extra_[1]);
		hue = params_.color1_;
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		// a colored dot sweeping back and forth, with fading trails
		fadeToBlackBy(gLeds, RGB_LED_COUNT, 20);
		int pos = beatsin16(params_.speed_, 0, RGB_LED_COUNT - 1);
		gLeds[pos] += CHSV(hue, 255, 192);
		hue += params_.extra_[0];
	}
};
