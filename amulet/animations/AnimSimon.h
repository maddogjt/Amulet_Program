#pragma once

#include "animation.h"
#include <type_traits>

enum class SimonColor
{
    None,
    Red,
    Green,
    Blue,
    Yellow
};

inline CRGB simon_color_to_rgb(SimonColor c)
{
    switch (c)
    {
    case SimonColor::Red:
        return CRGB::Red;
    case SimonColor::Green:
        return CRGB::Green;
    case SimonColor::Blue:
        return CRGB::Blue;
    case SimonColor::Yellow:
        return CRGB::Yellow;
    case SimonColor::None:
    default:
        return CRGB::Black;
    }
}

struct SimonStep
{
    SimonColor color;
    int32_t duration;
};

constexpr size_t kMaxSimonSteps = 32;

inline size_t generate_simon_sequence(uint8_t seed, uint8_t level, SimonStep out[kMaxSimonSteps])
{
    srand(seed);

    int steps = 4 + level * 2;
    for (int i = 0; i < steps; i++) {
        out[i*2].color = (SimonColor)(rand() % 4 + 1);
        out[i*2].duration = 420;

        out[i * 2 + 1].color = SimonColor::None;
        out[i * 2 + 1].duration = 50;
    }

    out[steps*2-1].duration=1500;

    return steps * 2;
}

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
