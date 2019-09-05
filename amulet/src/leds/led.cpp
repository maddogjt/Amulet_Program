#include "led.h"

#include "../settings/settings.h"

#include <FastLED.h>

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

LedBrightness g_brightness_mode = LedBrightness::Medium;
bool g_override_brightness_mode = false;

void led_set_brightness(LedBrightness brightness)
{
	g_brightness_mode = brightness;

	if (g_override_brightness_mode)
	{
		brightness = LedBrightness::High;
	}

	uint8_t newBrightness = 0;
	bool newLedPower = true;
	switch (brightness)
	{
	case LedBrightness::Low:
		newBrightness = localSettings_.brightness_[0];
		break;
	case LedBrightness::Medium:
		newBrightness = localSettings_.brightness_[1];
		break;
	case LedBrightness::High:
		newBrightness = localSettings_.brightness_[2];
		break;
	case LedBrightness::Off:
	case LedBrightness::Count:
	default:
		newLedPower = false;
	}

	if (newLedPower)
	{
		// Turn on LED power rail
		LOG_LV1("BRIT", "Setting brightness to %d (mode %d)", newBrightness, brightness);
		digitalWrite(PIN_RGB_LED_PWR, RGB_LED_PWR_ON);
		FastLED.setBrightness(newBrightness);
	}
	else
	{
		LOG_LV1("BRIT", "Turning off LED power rail (mode %d)", brightness);
		FastLED.setBrightness(newBrightness);
		// Turn off the LED power rail
		digitalWrite(PIN_RGB_LED_PWR, !RGB_LED_PWR_ON);

		// TODO: Also turn off bluetooth.
		// FEATURE: Maybe advertise in special off mode so we can find lost beacons by rssi?
	}
}

void led_next_brightness()
{
	led_set_brightness((LedBrightness)(((int)g_brightness_mode + 1) % (int)LedBrightness::Count));
}

void led_refresh_brightness()
{
	led_set_brightness(g_brightness_mode);
}

void led_override_brightness(bool over)
{
	g_override_brightness_mode = over;
	led_refresh_brightness();
}
