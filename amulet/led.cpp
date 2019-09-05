
#include <FastLED.h>

#include "globals.h"
#include "settings.h"
#include "BrightnessMode.h"

FASTLED_USING_NAMESPACE

#define DATA_PIN PIN_RGB_LEDS
#define BIKE_MODE_PIN 28
#define BIKE_LED_COUNT 200
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB

CRGB gLeds[RGB_LED_COUNT];

CRGB bikeModeLeds[BIKE_LED_COUNT];

void led_setup()
{
	//localSettings_.bikeMode_  = true;
	//if (localSettings_.bikeMode_)
	{
	//} else { 
		FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(gLeds, RGB_LED_COUNT).setCorrection(TypicalLEDStrip);
	}
	FastLED.addLeds<LED_TYPE, BIKE_MODE_PIN, COLOR_ORDER>(bikeModeLeds, BIKE_LED_COUNT).setCorrection(TypicalLEDStrip);
}

void led_loop(int step)
{
	for (int i = 0; i < BIKE_LED_COUNT; i++)
	{
		if (!localSettings_.bikeExtend_) {
			bikeModeLeds[i] = gLeds[i % RGB_LED_COUNT];
		} else {
			float pos = ((float)i)/BIKE_LED_COUNT;
			float orgPos = pos * RGB_LED_COUNT;
			int low = (int)floor(orgPos);
			float frac = orgPos-low;
			bikeModeLeds[i] = gLeds[low].lerp8(gLeds[low+1], frac*255);
		}
	}

	FastLED.show();
}
