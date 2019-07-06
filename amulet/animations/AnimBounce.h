#pragma once

#include "../Animation.h"

class AnimBounce : public Animation
{
public:
	uint8_t hue;
	uint8_t extra0;
	uint8_t extra1;
	virtual void init()
	{
		LOG_LV1("LED", "Bounce::init");
		hue = params_.color1_;
		extra0 = params_.extra0_;
		extra1 = params_.extra1_;
		fill_solid(leds, RGB_LED_COUNT, CHSV(hue, 255, 0));
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{ //This does there and back
		fill_solid(leds, RGB_LED_COUNT, CHSV(hue, 255, 0));
		if (extra1 == 0)
		{
			leds[extra0] = CHSV(params_.color2_, 255, 192);
			extra0++;
			if (extra0 == 8)
			{
				extra0 = 7;
				extra1 = 1;
			}
		}
		else
		{
			leds[extra0] = CHSV(params_.color2_, 255, 192);
			if (extra0 == 0)
			{
				extra1 = 0;
			}
			else
			{
				extra0--;
			}
		}
		delay(100);
	}
};