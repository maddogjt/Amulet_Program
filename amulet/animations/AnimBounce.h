#pragma once

#include "../Animation.h"

class AnimBounce : public Animation
{
public:
	uint8_t hue;
	virtual void init()
	{
		LOG_LV1("LED", "Bounce::init( %d, %d )", params_.extra0_, params_.extra1_);
		hue = params_.color1_;
		fill_solid(leds, RGB_LED_COUNT, CHSV(hue, 255, 0));
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{ //This does there and back
		fill_solid(leds, RGB_LED_COUNT, CHSV(hue, 255, 0));
		if (params_.extra1_ == 0)
		{
			leds[params_.extra0_] = CHSV(params_.color2_, 255, 192);
			params_.extra0_++;
			if (params_.extra0_ == 8)
			{
				params_.extra0_ = 7;
				params_.extra1_ = 1;
			}
		}
		else
		{
			leds[params_.extra0_] = CHSV(params_.color2_, 255, 192);
			if (params_.extra0_ == 0)
			{
				params_.extra1_ = 0;
			}
			else
			{
				params_.extra0_--;
			}
		}
		delay(100);
	}
};