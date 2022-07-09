#pragma once

#include "animation.h"

class AnimJump : public Animation
{
public:
	int hue;
	int hue2;
	uint8_t extra0;
	uint8_t extra1;

	virtual void init()
	{
		LOG_LV1("LED", "Jump::init");
		hue = params_.color1_;
		hue2 = params_.color2_;
		extra0 = params_.extra0_;
		extra1 = params_.extra1_;

		fill_solid(leds, RGB_LED_COUNT, CHSV(hue, 255, 0));
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		fadeToBlackBy(leds, RGB_LED_COUNT, 10);
		EVERY_N_MILLISECONDS(100 - extra1 / 5)
		{
			if ((extra1 % 48) >= 24)
			{
				leds[extra0] = CHSV((hue2 += 30), 255, 192);
				extra0++;
				if (extra0 == 8)
				{
					extra0 = 0;
				}
			}
			else
			{
				//clockwise
				leds[extra0] = CHSV((hue2 += 30), 255, 192);
				if (extra0 == 0)
				{
					extra0 = 8;
				}
				extra0--;
			}

			extra1++;
		}
	}
};