#pragma once

#include "../Animation.h"

class AnimConfetti : public Animation
{
public:
	virtual void init()
	{
		LOG_LV1("LED", "Confetti::init( %d, %d )", params_.extra_[0], params_.extra_[1]);
		gHue = params_.color1_;
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		fadeToBlackBy(gLeds, RGB_LED_COUNT, 10);
		int pos = random16(RGB_LED_COUNT);
		gLeds[pos] += CHSV(gHue + random8(64), 200, 255);
	}
};
