#pragma once

#include "../Animation.h"

class AnimTwinkle : public Animation
{
public:
	int hue;
	virtual void init()
	{
		LOG_LV1("LED", "Twinkle::init( %d, %d )", params_.extra0_, params_.extra1_);
		hue = params_.color1_;
		fill_solid(leds, RGB_LED_COUNT, CHSV(hue, 255, 0));
	}

	virtual void addGlitter(fract8 chanceOfVoids)
	{
		for (int i = 0; i < 3; i++)
		{
			if (random8() < chanceOfVoids)
			{
				leds[random16(RGB_LED_COUNT)] -= CRGB::White;
			}
		}
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		fadeToBlackBy(leds, RGB_LED_COUNT, 110);
		leds[random8(0, 4)] = CHSV((params_.color2_ += 30), 255, 192);
		leds[random8(5, 8)] = CHSV((params_.color2_ += 30), 255, 192);

		//addGlitter(80);
		delay(100);
	}
};