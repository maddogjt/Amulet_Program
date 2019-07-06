#pragma once

#include "../Animation.h"

class AnimInNOut : public Animation
{
public:
	int hue;
	virtual void init()
	{
		LOG_LV1("LED", "InNOut::init( %d, %d )", params_.extra0_, params_.extra1_);
		hue = params_.color1_;
		fill_solid(leds, RGB_LED_COUNT, CHSV(hue, 255, 0));
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		int outer[] = {0, 3, 4, 7};
		int inner[] = {1, 2, 5, 6};

		fadeToBlackBy(leds, RGB_LED_COUNT, 60);
		// Want to alternate turning off outer 4, then inner 4

		if (params_.extra0_ == 150)
		{
			if (params_.extra1_ == 0)
			{
				params_.extra1_ = 1;
			}
			else
			{
				params_.extra1_ = 0;
			}
			params_.extra0_ = 0;
		}

		if (params_.extra1_ == 0)
		{
			for (int i = 0; i < 4; i++)
			{
				leds[outer[i]] = CHSV((params_.color1_), 255, 192);
			}
		}
		else
		{
			for (int i = 0; i < 4; i++)
			{
				leds[inner[i]] = CHSV((params_.color1_), 255, 192);
			}
		}
		params_.extra0_++;
		params_.color1_ += 2;
	}
};