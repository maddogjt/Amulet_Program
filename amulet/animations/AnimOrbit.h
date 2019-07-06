#pragma once

#include "../Animation.h"

class AnimOrbit : public Animation
{
public:
	int hue;
	virtual void init()
	{
		LOG_LV1("LED", "Orbit::init( %d, %d )", params_.extra0_, params_.extra1_);
		hue = params_.color1_;
		fill_solid(leds, RGB_LED_COUNT, CHSV(hue, 255, 0));
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		int index_1 = 0;
		fadeToBlackBy(leds, RGB_LED_COUNT, 180);
		//This does corners

		switch (params_.extra0_)
		{
		case 0:
			break;
		case 1:
			index_1 = 3;
			break;
		case 2:
			index_1 = 4;
			break;
		case 3:
			index_1 = 7;
			break;
		}

		leds[index_1] = CHSV((params_.color2_ += 30), 255, 192);

		leds[1] = CHSV((params_.color1_), 255, 192);
		leds[2] = CHSV((params_.color1_), 255, 192);
		leds[5] = CHSV((params_.color1_), 255, 192);
		leds[6] = CHSV((params_.color1_), 255, 192);

		params_.extra0_++;
		if (params_.extra0_ == 4)
		{
			params_.extra0_ = 0;
			params_.color1_ += 11;
		}

		delay(100);
	}
};