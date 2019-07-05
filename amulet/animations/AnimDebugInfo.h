#pragma once

#include "../Animation.h"

class AnimDebugInfo : public Animation
{
public:
	virtual void init()
	{
		LOG_LV1("LED", "DebugInfo::init( %d, %d )", params_.extra0_, params_.extra1_);
		fill_solid(leds, RGB_LED_COUNT, CHSV(params_.color1_, 255, 0));
		fill_solid(leds, max(1, min(RGB_LED_COUNT, params_.extra0_)), CHSV(params_.color1_, 255, 64));
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		// Pulse the brightness between 64 and 192
		fill_solid(leds, max(1, min(RGB_LED_COUNT, params_.extra0_)), CHSV(params_.color1_, 255, 64 + sin8(frame) / 2));
	}
};
