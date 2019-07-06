#pragma once

#include "../Animation.h"

class AnimBounce : public Animation
{
public:
	virtual void init()
	{
		LOG_LV1("LED", "Bounce::init( %d, %d )", params_.extra_[0], params_.extra_[1]);
		gHue = params_.color1_;
		fill_solid(gLeds, RGB_LED_COUNT, CHSV(gHue, 255, 0));
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{	//This does there and back
		fill_solid(gLeds, RGB_LED_COUNT, CHSV(gHue, 255, 0));
		if( params_.extra_[1] == 0 ) {
			gLeds[params_.extra_[0]] = CHSV(params_.color2_, 255, 192);
			params_.extra_[0]++;
			if( params_.extra_[0] == 8 ) {
				params_.extra_[0] = 7;
				params_.extra_[1] = 1;
			}
		} else {
			gLeds[params_.extra_[0]] = CHSV(params_.color2_, 255, 192);
			if( params_.extra_[0] == 0 ) {
				params_.extra_[1] = 0;
			} else {
				params_.extra_[0]--;
			}
		}
		delay(100);
	}		
};
