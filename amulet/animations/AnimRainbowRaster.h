#pragma once

#include "../Animation.h"

#include "RasterHelper.h"

// This maps a rainbow in a 2d space at a given angle
//
//
// fHue is the base color of the rainbow.
// Using floating point hue gives more precision even though integer hue is used in the end.
//
// if angle == 0:
// Hue(x,y) = x + fHue;
//
// if angle == π/2
// Hue(x,y) = y + fHue;
//
// Other angles cause more of a blend :)

static float angle = PI / 2; //0.2; //0; //PI / 4;
static float fHue = 0;
class AnimRainbowRaster : public Animation
{
public:
	virtual void init()
	{
		LOG_LV1("LED", "AnimRainbowRaster::init( %d, %d )", params_.extra0_, params_.extra1_);
		fHue = (float_t)params_.color1_;
		for (int i = 0; i < RGB_LED_COUNT; i++)
		{
			leds[i] = CHSV((int)fHue, 255, 255);
		}
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		fHue += 0.3;
		angle += 0.01;

		float x;
		float y;
		for (int i = 0; i < RGB_LED_COUNT; i++)
		{
			x = led_positions()[i].x + 5; // need to offset by 5 to avoid 0s mucking up math
			y = led_positions()[i].y + 5;
			float alpha = atanf(y / x);
			// Serial.printf("alpha: %f  (y:%f / x:%f)\n", alpha, y, x);
			float c = distance(0, x, 0, y);
			// Serial.printf("c: %f     angle: %f\n", c, angle);

			float hue = c * cosf(alpha - angle) + fHue;
			int ihue = (int)(hue + 255) % 255;
			leds[i] = CHSV(ihue, 255, 192);
		}
	}
};
