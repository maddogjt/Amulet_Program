#pragma once

#include "../Animation.h"

class AnimCylon : public Animation
{
public:
	int hue;
	int speed;
	virtual void init()
	{
		LOG_LV1("LED", "Cylon::init");
		hue = params_.color1_;
		speed = max(2, params_.speed_ / 8);
		fill_solid(leds, RGB_LED_COUNT, CHSV(hue, 255, 0));
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		// a colored dot sweeping back and forth, with fading trails
		fadeToBlackBy(leds, 4, 20);
		int pos = beatsin16(speed, 0, 4 - 1);
		leds[pos] += CHSV(hue, 255, 192);
		mirror();
		hue += params_.extra0_ / 16;
	}
};
