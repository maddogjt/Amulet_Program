#include "led.h"

#include "../settings/settings.h"
#include "../misc/system_sleep.h"

#include <FastLED.h>

#define DATA_PIN PIN_RGB_LEDS
#define BIKE_MODE_PIN 28
#define BIKE_LED_COUNT 200
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB

CRGB gLeds[RGB_LED_COUNT];
CRGB bikeModeLeds[BIKE_LED_COUNT];

uint32_t gTurnOffTime = 0;
bool gWaitingToSleep = false;

LedBrightness g_brightness_mode = LedBrightness::Medium;
uint8_t g_override_brightness_value = 0;
uint8_t g_old_brightness = 0;


void led_setup()
{
#if !defined(NO_RGB_LEDS)
	pinMode(PIN_RGB_LED_PWR, OUTPUT);
	digitalWrite(PIN_RGB_LED_PWR, RGB_LED_PWR_ON);

	//localSettings_.bikeMode_  = true;
	//if (localSettings_.bikeMode_)
	{
	//} else { 
		FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(gLeds, RGB_LED_COUNT).setCorrection(TypicalLEDStrip);
	}
	FastLED.addLeds<LED_TYPE, BIKE_MODE_PIN, COLOR_ORDER>(bikeModeLeds, BIKE_LED_COUNT).setCorrection(TypicalLEDStrip);
#endif
}

LedBrightness led_get_brightness()
{
	return g_brightness_mode;
}

void led_loop(int step)
{
#if !defined(NO_RGB_LEDS)
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

	if ( g_brightness_mode != LedBrightness::Off ) {
		FastLED.show();
	} else if( gWaitingToSleep && gTurnOffTime + 30000 < millis() ) {
		Serial.println("[LED] Going to sleep now");
		// TODO: Only do this in blinky mode
		gWaitingToSleep = false;
		write_local_settings();
		power_off();
	}
#endif
}

void led_set_brightness(LedBrightness brightness)
{
	g_brightness_mode = brightness;

	uint8_t newBrightness = 0;

	if (g_override_brightness_value)
	{
		newBrightness = g_override_brightness_value;
	} 
	else 
	{
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
			break;
		}
	}


#if !defined(NO_RGB_LEDS) 
	if (newBrightness > 0)
	{
		// Turn on LED power rail
		if ( newBrightness != g_old_brightness ) {
			LOG_LV1("BRIT", "Setting brightness to %d (mode %d)", newBrightness, brightness);
		}
		digitalWrite(PIN_RGB_LED_PWR, RGB_LED_PWR_ON);
		FastLED.setBrightness(newBrightness);
		gWaitingToSleep = false;
	}
	else
	{
		if ( newBrightness != g_old_brightness ) {
			LOG_LV1("BRIT", "Turning off LED power rail (mode %d)", brightness);

			// Only blinkies turn off after 30s, the other modes keep on and keep advertising.
			gWaitingToSleep = (localSettings_.startupConfig_.mode==AMULET_MODE_BLINKY);
			gTurnOffTime = millis();
		}
		FastLED.setBrightness(newBrightness);
		// Turn off the LED power rail
		digitalWrite(PIN_RGB_LED_PWR, !RGB_LED_PWR_ON);

		// TODO: Also turn off bluetooth.
		// FEATURE: Maybe advertise in special off mode so we can find lost beacons by rssi?
	}
	g_old_brightness = newBrightness;
#endif
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
	g_override_brightness_value = over ? localSettings_.brightness_[2] : 0;
	led_refresh_brightness();
}

void led_override_brightness_value(uint8_t brightness)
{
	g_override_brightness_value = brightness;
	led_refresh_brightness();
}