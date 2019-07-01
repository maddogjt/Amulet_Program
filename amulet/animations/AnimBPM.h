#pragma once

#include "../Animation.h"

class AnimBPM : public Animation
{
public:
	virtual void init()
	{
		LOG_LV1("LED", "Bpm::init( %d, %d )", animation_p1, animation_p2);
		gHue = animation_p1;
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		// colored stripes pulsing at a defined Beats-Per-Minute (BPM)
		uint8_t BeatsPerMinute = 62;
		CRGBPalette16 palette = PartyColors_p;
		uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
		for (int i = 0; i < RGB_LED_COUNT; i++)
		{ //9948
			gLeds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
		}
		gHue += animation_p2;
	}
};
