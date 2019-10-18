#pragma once

#include "animation.h"
#include "../src/misc/simon_sequence.h"
#include <type_traits>

class AnimSimon : public Animation
{
public:
	virtual void init()
	{
		LOG_LV1("LED", "Simon::init");
		simonStepCount_ = generate_simon_sequence(
			params_.extra0_, params_.speed_ * 4, params_.extra1_,
			simonSteps_);
		startStep(0);
	}

	void startStep(int stepIndex)
	{
		currentSimonStep_ = stepIndex % simonStepCount_;
		stepEnd_ = millis() + simonSteps_[currentSimonStep_].duration;
		CRGB color = simon_color_to_rgb(simonSteps_[currentSimonStep_].color);

		uint8_t ledEnableMask[5] = {0,
									0x0C,
									0x30,
									0x03,
									0xC0};
		uint8_t mask = ledEnableMask[(int)simonSteps_[currentSimonStep_].color];

		for (int i = 0; i < RGB_LED_COUNT; i++)
		{
			if ((mask & (1 << i)) != 0)
			{
				leds[i] = color;
			}
			else
			{
				leds[i] = CRGB::Black;
			}
		}
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		if (millis() >= stepEnd_)
		{
			startStep(currentSimonStep_ + 1);
		}
	}

	SimonStep simonSteps_[kMaxSimonSteps];
	size_t simonStepCount_ = 0;
	size_t currentSimonStep_ = 0;
	int stepEnd_ = 0;
};
