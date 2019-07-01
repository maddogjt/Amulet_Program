#pragma once

#include "../Animation.h"

#include <FastLED.h>
FASTLED_USING_NAMESPACE

class AnimFlame : public Animation
{
public:
	virtual void init()
	{
		LOG_LV1("LED", "Flame::init( %d, %d )", animation_p1, animation_p2);
		gPal = HeatColors_p;
        fill_solid(gLeds, RGB_LED_COUNT, CHSV(gHue, 255, 192));
    }

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		// Add entropy to random number generator; we use a lot of it.
		random16_add_entropy(random());
		Fire2012WithPalette(); // run simulation frame, using palette colors
		mirror();
		// do the animation thing here
	}

	// Flame: Adapted from https://github.com/FastLED/FastLED/blob/master/examples/Fire2012WithPalette/Fire2012WithPalette.ino
	// p1 :
	// p2 :
	bool gReverseDirection = false;
	CRGBPalette16 gPal;

#define COOLING 30
#define SPARKING 180
	void Fire2012WithPalette()
	{
		// Array of temperature readings at each simulation cell
        static byte heat[RGB_LED_COUNT];

        // Step 1.  Cool down every cell a little
        for (int i = 0; i < RGB_LED_COUNT; i++)
        {
            heat[i] = qsub8(heat[i], random8(0, ((COOLING * 10) / RGB_LED_COUNT) + 2));
        }

		// Step 2.  Heat from each cell drifts 'up' and diffuses a little
        for (int k = RGB_LED_COUNT - 1; k >= 2; k--)
        {
			heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
		}

		// Step 3.  Randomly ignite new 'sparks' of heat near the bottom
		if (random8() < SPARKING)
		{
			int y = random8(7);
			heat[y] = qadd8(heat[y], random8(160, 255));
		}

		// Step 4.  Map from heat cells to LED colors
        for (int j = 0; j < RGB_LED_COUNT; j++)
        {
			// Scale the heat value from 0-255 down to 0-240
			// for best results with color palettes.
			byte colorindex = scale8(heat[j], 240);
			CRGB color = ColorFromPalette(gPal, colorindex);
			int pixelnumber;
			if (gReverseDirection)
			{
                pixelnumber = (RGB_LED_COUNT - 1) - j;
            }
			else
			{
				pixelnumber = j;
			}
			gLeds[pixelnumber] = color;
		}
	}
};