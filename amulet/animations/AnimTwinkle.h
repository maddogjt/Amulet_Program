#pragma once

#include "../Animation.h"

class AnimTwinkle : public Animation
{
public:
	virtual void init()
	{
		LOG_LV1("LED", "Twinkle::init( %d, %d )", params_.extra_[0], params_.extra_[1]);
		gHue = params_.color1_;
		fill_solid(gLeds, RGB_LED_COUNT, CHSV(gHue, 255, 0));
	}

	virtual void addGlitter(fract8 chanceOfVoids)
	{
		for ( int i = 0; i < 3; i++) {
			if (random8() < chanceOfVoids) {
				gLeds[random16(RGB_LED_COUNT)] -= CRGB::White;
			}
		}
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		fadeToBlackBy(gLeds, RGB_LED_COUNT, 110);
		gLeds[random8(0,4)] = CHSV((params_.color2_ += 30), 255, 192);
		gLeds[random8(5,8)] = CHSV((params_.color2_ += 30), 255, 192);

		//addGlitter(80);
		delay(100);
	}
};
