#pragma once

#include "../Animation.h"
#include "RasterHelper.h"

class AnimBallRaster : public Animation
{
public:
	virtual void init()
	{
		LOG_LV1("LED", "AnimBallRaster::init( %d, %d )", params_.extra_[0], params_.extra_[1]);
		gHue = params_.extra_[0];

		for (int i = 0; i < RGB_LED_COUNT; i++)
		{
			gLeds[i] = CHSV(gHue, 255, 255);
		}
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		int32_t xval = beatsin16(11, 0, 122 + 50) - 25;
		int32_t yval = beatsin16(14, 0, 156 + 50) - 25;
		EVERY_N_MILLISECONDS(150)
		gHue++;
		for (int i = 0; i < RGB_LED_COUNT; i++)
		{
			float_t dist = distance(led_positions[i].x, (float_t)xval, led_positions[i].y, (float_t)yval);
			dist = max(0, min(255, dist * 1.5 - 35));
			gLeds[i] = CHSV(gHue, 255, (int)dist);
		}
	}
};
