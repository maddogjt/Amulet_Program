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
        simonStepCount_ = generate_simon_sequence(42, 1, simonSteps_);
        startStep(0);
    }

    void startStep(int stepIndex) {
        currentSimonStep_ = stepIndex % simonStepCount_;
        stepEnd_ = millis() + simonSteps_[currentSimonStep_].duration;
        CRGB color = simon_color_to_rgb(simonSteps_[currentSimonStep_].color);

        for (int i = 0; i < RGB_LED_COUNT; i++)
        {
            leds[i] = color;
        }
    }

    virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
    {
        if (millis() >= stepEnd_) {
            startStep(currentSimonStep_+1);
        }
            }

    SimonStep simonSteps_[kMaxSimonSteps];
    size_t simonStepCount_ = 0;
    size_t currentSimonStep_ = 0;
    int stepEnd_ = 0;
};
