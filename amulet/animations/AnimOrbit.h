#pragma once

#include "../Animation.h"

class AnimOrbit : public Animation
{
public:
	virtual void init()
	{
		LOG_LV1("LED", "Orbit::init( %d, %d )", params_.extra_[0], params_.extra_[1]);
		gHue = params_.color1_;
		fill_solid(gLeds, RGB_LED_COUNT, CHSV(gHue, 255, 0));
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		int index_1 = 0;
		fadeToBlackBy(gLeds, RGB_LED_COUNT, 180);
		//This does corners
		
		switch( params_.extra_[0] ) {
			case 0:
				break;
			case 1:
				index_1 = 3;
				break;
			case 2:
				index_1 = 4;
				break;
			case 3:
				index_1 = 7;
				break;
		}
		
		gLeds[index_1] = CHSV((params_.color2_ += 30), 255, 192);
		
		gLeds[1] = CHSV((params_.color1_ ), 255, 192);
		gLeds[2] = CHSV((params_.color1_ ), 255, 192);
		gLeds[5] = CHSV((params_.color1_ ), 255, 192);
		gLeds[6] = CHSV((params_.color1_ ), 255, 192);
		
		params_.extra_[0]++;
		if( params_.extra_[0] == 4 ) {
				params_.extra_[0] = 0;
				params_.color1_ += 11;
		}
		
		delay(100);
	}
};
