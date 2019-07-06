#pragma once

#include "../Animation.h"

#define ARDUINO_GENERIC
#include <FastLED.h>
#undef ARDUINO_GENERIC
FASTLED_USING_NAMESPACE

class AnimBurn : public Animation
{
public:
	virtual void init()
	{
		LOG_LV1("LED", "Burn::init");
		gPal = HeatColors_p;
		fill_solid(leds, RGB_LED_COUNT, CHSV(0, 255, 0));
	}

	CRGBPalette16 gPal;
	int height[2] = {};
	int low_temp[2] = {};
	int high_temp[2] = {};

	int heat[2][4] = {0}; // 2 columns, 4 rows
	CRGB *colRowLeds[2][4] = {
		{&leds[7], &leds[6], &leds[5], &leds[4]},
		{&leds[0], &leds[1], &leds[2], &leds[3]}};
	CRGB rowColLeds[4][2] = {};

	// int heights[2] = {0};
	// int baseHeat = 0;

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		fadeToBlackBy(&rowColLeds[0][0], 8, 40);

		for (int i = 0; i < 2; i++)
		{
			height[i] = min(3, beatsin16(40 + i, 0, 3) + beatsin16(29 - i, 0, 2));
			low_temp[i] = beatsin16(37 + i, 1, 3) + beatsin16(13 - i, 0, 3);
			high_temp[i] = min(10, low_temp[i] + beatsin16(7 + i, 0, 5) + beatsin16(3 - i, 0, 4));
		}

		for (int row = 0; row < 4; row++)
		{
			for (int col = 0; col < 2; col++)
			{
				if (height[col] != 0 && height[col] >= row)
				{
					uint8_t heat = lerp8by8(low_temp[col], high_temp[col], (row * 255 / height[col]));
					rowColLeds[row][col] = gPal[heat];
				}
			}
		}

		// const int spread = 5;
		// for (int row = 0; row < 4; row++)
		// {
		// 	blur1d(rowColLeds[row], 2, spread);
		// }

		leds[0] = rowColLeds[0][1];
		leds[1] = rowColLeds[1][1];
		leds[2] = rowColLeds[2][1];
		leds[3] = rowColLeds[3][1];
		leds[4] = rowColLeds[3][0];
		leds[5] = rowColLeds[2][0];
		leds[6] = rowColLeds[1][0];
		leds[7] = rowColLeds[0][0];
	}
};
