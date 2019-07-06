#pragma once

#include "../Animation.h"

class AnimJump : public Animation
{
public:
	int hue;
	virtual void init()
	{
		LOG_LV1("LED", "Jump::init( %d, %d )", params_.extra0_, params_.extra1_);
		hue = params_.color1_;
		fill_solid(leds, RGB_LED_COUNT, CHSV(hue, 255, 0));
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{

		// Want to alternate turning off outer 4, then inner 4

		/*
 		//This does there and back
 		fill_solid(leds, RGB_LED_COUNT, CHSV(hue, 255, 0));
 		if( params_.extra1_ == 0 ) {
 			leds[params_.extra0_] = CHSV(params_.color2_, 255, 192);
 			params_.extra0_++;
 			if( params_.extra0_ == 8 ) {
 				params_.extra0_ = 7;
 				params_.extra1_ = 1;
 			}
 		} else {
 			leds[params_.extra0_] = CHSV(params_.color2_, 255, 192);
 			if( params_.extra0_ == 0 ) {
 				params_.extra1_ = 0;
 			} else {
 				params_.extra0_--;
 			}
 		}*/

		/*
 		//This does around the world counter clockwise
 		fill_solid(leds, RGB_LED_COUNT, CHSV(hue, 255, 0));
 		leds[params_.extra0_] = CHSV((params_.color2_ += 30), 255, 192);
 		params_.extra0_++;
 		if( params_.extra0_ == 8 ) {
 				params_.extra0_ = 0;
 		}
 		
 		delay(100);
 		*/

		/*
 		//This does around the world clockwise
 		fill_solid(leds, RGB_LED_COUNT, CHSV(hue, 255, 0));
 		leds[params_.extra0_] = CHSV((params_.color2_ += 30), 255, 192);
 		if( params_.extra0_ == 0 ) {
 				params_.extra0_ = 8;
 		}
 		params_.extra0_--;
 		
 		delay(100);
 		*/

		//This goes around back and forth
		//counter clockwise
		// p.extra0_ needs to start as 8
		fadeToBlackBy(leds, RGB_LED_COUNT, 140);
		if ((params_.extra1_ % 48) >= 24)
		{
			leds[params_.extra0_] = CHSV((params_.color2_ += 30), 255, 192);
			params_.extra0_++;
			if (params_.extra0_ == 8)
			{
				params_.extra0_ = 0;
			}
		}
		else
		{
			//clockwise
			leds[params_.extra0_] = CHSV((params_.color2_ += 30), 255, 192);
			if (params_.extra0_ == 0)
			{
				params_.extra0_ = 8;
			}
			params_.extra0_--;
		}
		delay(100 - params_.extra1_ / 5);
		params_.extra1_++;
		/* 
 		//This does around the world with a tail
 		fadeToBlackBy(leds, RGB_LED_COUNT, 140);
 		leds[params_.extra0_] = CHSV((params_.color2_ += 30), 255, 192);
 		params_.extra0_++;
 		if( params_.extra0_ == 8 ) {
 				params_.extra0_ = 0;
 		}
 		delay(100);
 		*/
	}
};