#pragma once

#include "../Animation.h"

class AnimCylon : public Animation
{
public:
	int hue;
	virtual void init()
	{
		LOG_LV1("LED", "Cylon::init( %d, %d )", params_.extra0_, params_.extra1_);
		hue = params_.color1_;
		fill_solid(leds, RGB_LED_COUNT, CHSV(hue, 255, 0));
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		// a colored dot sweeping back and forth, with fading trails
		fadeToBlackBy(leds, 4, 20);
		int pos = beatsin16(params_.speed_, 0, 4 - 1);
		leds[pos] += CHSV(hue, 255, 192);
		mirror();
		hue += params_.extra0_ / 16;
	}
};
