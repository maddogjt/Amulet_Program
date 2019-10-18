#pragma once

#include "animation.h"

class AnimSafe : public Animation
{
public:
	float hue;
	virtual void init()
	{
		LOG_LV1("LED", "Safe::init");
		hue = params_.color1_;
		fill_solid(leds, RGB_LED_COUNT, CHSV(hue, 255, 192));
	}

	void startStep(int stepIndex)
	{
		currentSafeStep_ = stepIndex % safeStepCount_;
		stepEnd_ = millis() + 1500;

		int startIndex = random8() % 8;
		for (int i = 0; i < RGB_LED_COUNT; i++)
		{
			if (i < combo_[currentSafeStep_])
			{
				leds[(i + startIndex) % 8] = CRGB::Red;
			}
		}
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		fadeToBlackBy(leds, RGB_LED_COUNT, 10);
		if (millis() >= stepEnd_)
		{
			startStep(currentSafeStep_ + 1);
		}
	}

	size_t safeStepCount_ = 6;
	size_t currentSafeStep_ = 0;
	int stepEnd_ = 0;
	int combo_[6] = {2, 3, 5, 7, 0, 0};
};
