#pragma once

#include "../Animation.h"

class AnimRainbow : public Animation
{
public:
	virtual void init()
	{
		LOG_LV1("LED", "Rainbow::init( %d, %d )", params_.extra_[0], params_.extra_[1]);
		gHue = params_.color1_;
		fill_rainbow(gLeds, RGB_LED_COUNT, gHue++, 20);
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		gHue += params_.extra_[0];
		fill_rainbow(gLeds, RGB_LED_COUNT, gHue++, 20);
	}
};
