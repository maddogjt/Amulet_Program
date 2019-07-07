#pragma once

#include "../Animation.h"

class AnimInNOut : public Animation
{
public:
	int hue;
	int hue2;
	uint8_t extra0;
	uint8_t extra1;

	virtual void init()
	{
		LOG_LV1("LED", "InNOut::init");
		hue = params_.color1_;
		hue2 = params_.color2_;
		extra0 = params_.extra0_;
		extra1 = params_.extra1_;

		fill_solid(leds, RGB_LED_COUNT, CHSV(hue, 255, 0));
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		int outer[] = {0, 3, 4, 7};
		int inner[] = {1, 2, 5, 6};

		fadeToBlackBy(leds, RGB_LED_COUNT, 60);
		// Want to alternate turning off outer 4, then inner 4

		if (extra0 == 150)
		{
			if (extra1 == 0)
			{
				extra1 = 1;
			}
			else
			{
				extra1 = 0;
			}
			extra0 = 0;
		}

		if (extra1 == 0)
		{
			for (int i = 0; i < 4; i++)
			{
				leds[outer[i]] = CHSV((hue), 255, 192);
			}
		}
		else
		{
			for (int i = 0; i < 4; i++)
			{
				leds[inner[i]] = CHSV((hue), 255, 192);
			}
		}
		extra0++;
		hue += 2;
	}
};