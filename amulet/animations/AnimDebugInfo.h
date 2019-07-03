#pragma once

#include "../Animation.h"

class AnimDebugInfo : public Animation
{
public:
	virtual void init()
	{
		LOG_LV1("LED", "DebugInfo::init( %d, %d )", params_.extra_[0], params_.extra_[1]);
		gHue = params_.extra_[1];
		fill_solid(gLeds, RGB_LED_COUNT, CHSV(gHue, 255, 0));
		fill_solid(gLeds, max(1,min(RGB_LED_COUNT,params_.extra_[0])), CHSV(gHue, 255, 64));
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		// Pulse the brightness between 64 and 192
		fill_solid(gLeds, max(1,min(RGB_LED_COUNT,params_.extra_[0])), CHSV(gHue, 255, 64 + sin8(frame)/2));
	}
};
