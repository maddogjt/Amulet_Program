#pragma once

#include "animation.h"

class AnimDisplayInt : public Animation
{
public:
	uint8_t extra0;

	virtual void init()
	{
		LOG_LV1("LED", "AnimDisplayInt::init");
		extra0 = params_.extra0_;
		display(extra0);
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		display(extra0);
	}

	void display(const int number)
	{
		CRGB groupColor;
		if (number < 4) {	
			groupColor = CRGB::Blue; 
		} else if (number < 8) {
			groupColor = CRGB::Green; 
		} else if (number < 12) {
			groupColor = CRGB::Purple; 
		}else if (number < 16) {
			groupColor = CRGB::Red; 
		} else {
			groupColor = CRGB::Yellow; 
		}
		
		int num = number % 16;
		leds[4] = num < 4 ? groupColor : CRGB::Black;
		leds[5] = num < 8 && num >= 4 ? groupColor : CRGB::Black;
		leds[6] = num < 12 && num >= 8 ? groupColor : CRGB::Black;
		leds[7] = num >=12 ? groupColor : CRGB::Black;

		leds[3] = num%4 == 0 ? groupColor : CRGB::Black;
		leds[2] = num%4 == 1 ? groupColor : CRGB::Black;
		leds[1] = num%4 == 2 ? groupColor : CRGB::Black;
		leds[0] = num%4 == 3 ? groupColor : CRGB::Black;
	}
};
