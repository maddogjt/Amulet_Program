#pragma once

#include "../Animation.h"

class AnimJump : public Animation
{
public:
	virtual void init()
	{
		LOG_LV1("LED", "Jump::init( %d, %d )", params_.extra_[0], params_.extra_[1]);
		gHue = params_.color1_;
		fill_solid(gLeds, RGB_LED_COUNT, CHSV(gHue, 255, 0));
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		
		// Want to alternate turning off outer 4, then inner 4
		
		/*
		//This does there and back
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
		}*/
		
		/*
		//This does around the world counter clockwise
		fill_solid(gLeds, RGB_LED_COUNT, CHSV(gHue, 255, 0));
		gLeds[params_.extra_[0]] = CHSV((params_.color2_ += 30), 255, 192);
		params_.extra_[0]++;
		if( params_.extra_[0] == 8 ) {
				params_.extra_[0] = 0;
		}
		
		delay(100);
		*/
		
		/*
		//This does around the world clockwise
		fill_solid(gLeds, RGB_LED_COUNT, CHSV(gHue, 255, 0));
		gLeds[params_.extra_[0]] = CHSV((params_.color2_ += 30), 255, 192);
		if( params_.extra_[0] == 0 ) {
				params_.extra_[0] = 8;
		}
		params_.extra_[0]--;
		
		delay(100);
		*/
		
		//This goes around back and forth
		//counter clockwise
		// p.extra_[0] needs to start as 8
		fadeToBlackBy(gLeds, RGB_LED_COUNT, 140);
		if ( (params_.extra_[1] % 48) >= 24 ) {
			gLeds[params_.extra_[0]] = CHSV((params_.color2_ += 30), 255, 192);
			params_.extra_[0]++;
			if( params_.extra_[0] == 8 ) {
				params_.extra_[0] = 0;
			}
		} else {
			//clockwise
			gLeds[params_.extra_[0]] = CHSV((params_.color2_ += 30), 255, 192);
			if( params_.extra_[0] == 0 ) {
				params_.extra_[0] = 8;
			}
			params_.extra_[0]--;
		}
		delay(100 - params_.extra_[1] / 5);
		params_.extra_[1]++;
		/* 
		//This does around the world with a tail
		fadeToBlackBy(gLeds, RGB_LED_COUNT, 140);
		gLeds[params_.extra_[0]] = CHSV((params_.color2_ += 30), 255, 192);
		params_.extra_[0]++;
		if( params_.extra_[0] == 8 ) {
				params_.extra_[0] = 0;
		}
		delay(100);
		*/
	}
};
