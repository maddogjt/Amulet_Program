#pragma once

#include "animation.h"

class AnimScoreboard : public Animation
{
public:
	bool simonSolved_;
	bool safeSolved_;
	bool photokeySolved_;
	int hue;

	virtual void init()
	{
		LOG_LV1("LED", "Scoreboard::init");
		hue = params_.color1_;
		fill_solid(leds, RGB_LED_COUNT, CRGB::Black);
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		fadeToBlackBy(leds, RGB_LED_COUNT, 3);
		hue += params_.speed_ / 100.f;

		EVERY_N_MILLISECONDS(650)
		{
			int treePos = random8() % 8;
			leds[treePos] = CRGB::Purple;

			if (simonSolved_)
			{
				leds[random8() % 8] = CRGB::Blue;
			}

			if (safeSolved_)
			{
				leds[random8() % 8] = CRGB::Red;
			}

			if (photokeySolved_)
			{
				leds[random8() % 8] = CRGB::Green;
			}
		}
	}
};
