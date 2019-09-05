#pragma once

#include "animation.h"

#include "RasterHelper.h"

class AnimShadow : public Animation
{
public:
	uint8_t hue;
	virtual void init()
	{
		LOG_LV1("LED", "AnimShadow::init");
		hue = params_.color1_;

		for (int i = 0; i < RGB_LED_COUNT; i++)
		{
			leds[i] = CHSV(hue, 255, 255);
		}
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		int32_t xval = beatsin16(11, 0, 122 + 50) - 25;
		int32_t yval = beatsin16(14, 0, 156 + 50) - 25;
		EVERY_N_MILLISECONDS(150)
		hue++;
		for (int i = 0; i < RGB_LED_COUNT; i++)
		{
			float_t dist = distance(led_positions()[i].x, (float_t)xval, led_positions()[i].y, (float_t)yval);
			dist = max(0, min(255, dist * 1.5 - 35));
			leds[i] = CHSV(hue, 255, (int)dist);
		}
	}
};
