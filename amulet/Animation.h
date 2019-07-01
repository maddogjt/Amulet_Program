#pragma once

#include <cstdint>
#include <Arduino.h>
#include "globals.h"
#include <FastLED.h>
FASTLED_USING_NAMESPACE

struct animColor {
	uint8_t h;
	uint8_t s;
	uint8_t v;
};

struct animParams {
	animColor color1_;
	animColor color2_;
	animColor color3_;
	uint8_t speed_;
	uint8_t flags_;
	uint8_t extra_[6];
};

// TODO: remove?
extern uint8_t gHue;

extern CRGB gLeds[RGB_LED_COUNT];

void mirror();


class Animation
{
	public:
	virtual ~Animation(){}

	// temporary, switch to using params_
		int animation_p1;
		int animation_p2;

	public:
		// Animation(const animParams &parameters) : params_(parameters) {}
		void setParams(const animParams &parameters)
		{
			params_ = parameters;
			animation_p1 = params_.extra_[0];
			animation_p2 = params_.extra_[1];
	}
	  virtual void init() {}
	  virtual void step(const int frame, const float deltaTime, const float sourceDistance) = 0;

	  animParams params_;
};
