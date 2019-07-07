#pragma once

#include "../Animation.h"

class AnimJump : public Animation
{
public:
	int hue;
	int hue2;
	uint8_t extra0;
	uint8_t extra1;

	virtual void init()
	{
		LOG_LV1("LED", "Jump::init");
		hue = params_.color1_;
		hue2 = params_.color2_;
		extra0 = params_.extra0_;
		extra1 = params_.extra1_;

		fill_solid(leds, RGB_LED_COUNT, CHSV(hue, 255, 0));
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{

		// Want to alternate turning off outer 4, then inner 4

		/*
 		//This does there and back
 		fill_solid(leds, RGB_LED_COUNT, CHSV(hue, 255, 0));
 		if( extra1 == 0 ) {
 			leds[extra0] = CHSV(hue2, 255, 192);
 			extra0++;
 			if( extra0 == 8 ) {
 				extra0 = 7;
 				extra1 = 1;
 			}
 		} else {
 			leds[extra0] = CHSV(hue2, 255, 192);
 			if( extra0 == 0 ) {
 				extra1 = 0;
 			} else {
 				extra0--;
 			}
 		}*/

		/*
 		//This does around the world counter clockwise
 		fill_solid(leds, RGB_LED_COUNT, CHSV(hue, 255, 0));
 		leds[extra0] = CHSV((hue2 += 30), 255, 192);
 		extra0++;
 		if( extra0 == 8 ) {
 				extra0 = 0;
 		}
 		
 		delay(100);
 		*/

		/*
 		//This does around the world clockwise
 		fill_solid(leds, RGB_LED_COUNT, CHSV(hue, 255, 0));
 		leds[extra0] = CHSV((hue2 += 30), 255, 192);
 		if( extra0 == 0 ) {
 				extra0 = 8;
 		}
 		extra0--;
 		
 		delay(100);
 		*/

		//This goes around back and forth
		//counter clockwise
		// p.extra0_ needs to start as 8
		fadeToBlackBy(leds, RGB_LED_COUNT, 10);
		EVERY_N_MILLISECONDS(100 - extra1 / 5)
		{
			if ((extra1 % 48) >= 24)
			{
				leds[extra0] = CHSV((hue2 += 30), 255, 192);
				extra0++;
				if (extra0 == 8)
				{
					extra0 = 0;
				}
			}
			else
			{
				//clockwise
				leds[extra0] = CHSV((hue2 += 30), 255, 192);
				if (extra0 == 0)
				{
					extra0 = 8;
				}
				extra0--;
			}

			extra1++;
		}
		/* 
 		//This does around the world with a tail
 		fadeToBlackBy(leds, RGB_LED_COUNT, 140);
 		leds[extra0] = CHSV((hue2 += 30), 255, 192);
 		extra0++;
 		if( extra0 == 8 ) {
 				extra0 = 0;
 		}
 		delay(100);
 		*/
	}
};