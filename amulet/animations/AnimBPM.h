#pragma once

#include "../Animation.h"

class AnimBPM : public Animation
{
public:
	int hue;
	virtual void init()
	{
		LOG_LV1("LED", "Bpm::init( %d, %d )", params_.extra0_, params_.extra1_);
		hue = params_.color1_;
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		// colored stripes pulsing at a defined Beats-Per-Minute (BPM)
		uint8_t BeatsPerMinute = params_.speed_;
		CRGBPalette16 palette = PartyColors_p;
		uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
		for (int i = 0; i < RGB_LED_COUNT; i++)
		{ //9948
			leds[i] = ColorFromPalette(palette, hue + (i * 2), beat - hue + (i * 10));
		}
		hue += params_.extra0_ / 16;
	}
};
