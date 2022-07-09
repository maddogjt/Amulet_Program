#pragma once

#include "animation.h"
#include "../src/animation/animation_modifiers.h"

class AnimCylon : public Animation
{
public:
	int hue;
	int hueChange;
	int speed;
	virtual void init()
	{
		LOG_LV1("LED", "Cylon::init");
		hue = params_.color1_;
		hueChange = params_.extra0_ / 16;
		if (params_.extra1_) {
			speed = max(2, params_.speed_ / 8);
		} else {
			speed = 24;
			hueChange = -1;
		}
		fill_solid(leds, RGB_LED_COUNT, CHSV(hue, 255, 0));
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		// a colored dot sweeping back and forth, with fading trails
		fadeToBlackBy(leds, RGB_LED_COUNT, 20);
		int pos = beatsin16(speed, 0, 4 - 1);
		leds[pos] += CHSV(hue, 255, 192);
		leds[4+pos] += CHSV(hue, 255, 192);
		if ( (frame % 120) == 0) {
			hue += hueChange;
		}
	}
};
