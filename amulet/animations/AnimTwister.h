#pragma once

#include "../Animation.h"

class AnimTwister : public Animation
{
public:
	virtual void init()
	{	
		twisterColors[0] = CRGB::Green;
		twisterColors[1] = CRGB::Yellow;
		twisterColors[2] = CRGB::Blue;
		twisterColors[3] = CRGB::Red;

		LOG_LV1("LED", "Twister::init( %d, %d )", animation_p1, animation_p2);

		for( int i =0; i<RGB_LED_COUNT; i++) {
			gLeds[i] = twisterColors[random8() % ARRAY_SIZE(twisterColors)];
		}
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		if( frame % 24 == 0 ) {
			for( int i =0; i<RGB_LED_COUNT; i++) {
				if( random8() % 3 == 0 ) {
					gLeds[i] = twisterColors[random8() % ARRAY_SIZE(twisterColors)];
				}
			}
		}
	}

	CRGB twisterColors[4];
};
