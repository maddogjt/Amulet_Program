#include "BrightnessMode.h"

#define ARDUINO_GENERIC
#include <FastLED.h>
#undef ARDUINO_GENERIC

uint8_t g_led_brightness_low = 4;
uint8_t g_led_brightness_medium = 8;
uint8_t g_led_brightness_high = 20;

typedef enum
{
	AMULET_BRIGHTNESS_LOW,
	AMULET_BRIGHTNESS_MEDIUM,
	AMULET_BRIGHTNESS_HIGH,
	AMULET_BRIGHTNESS_OFF,
	AMULET_BRIGHTNESS_MODE_COUNT,
} amulet_brightness_mode_t;

amulet_brightness_mode_t g_brightness_mode = AMULET_BRIGHTNESS_MEDIUM;

void setBrightnessMode(amulet_brightness_mode_t mode)
{
	g_brightness_mode = mode;

	uint8_t newBrightness = 0;
	bool newLedPower = true;
	switch (mode)
	{
	case AMULET_BRIGHTNESS_LOW:
		newBrightness = g_led_brightness_low;
		break;
	case AMULET_BRIGHTNESS_MEDIUM:
		newBrightness = g_led_brightness_medium;
		break;
	case AMULET_BRIGHTNESS_HIGH:
		newBrightness = g_led_brightness_high;
		break;
	case AMULET_BRIGHTNESS_OFF:
	case AMULET_BRIGHTNESS_MODE_COUNT:
	default:
		newLedPower = false;
	}

	if (newLedPower)
	{
		// Turn on LED power rail
		LOG_LV1("BRIT", "Setting brightness to %d (mode %d)", newBrightness, mode);
		digitalWrite(PIN_RGB_LED_PWR, RGB_LED_PWR_ON);
		FastLED.setBrightness(newBrightness);
	}
	else
	{
		LOG_LV1("BRIT", "Turning off LED power rail (mode %d)", mode);
		// Turn off the LED power rail
		digitalWrite(PIN_RGB_LED_PWR, !RGB_LED_PWR_ON);

		// TODO: Also turn off bluetooth.
		// FEATURE: Maybe advertise in special off mode so we can find lost beacons by rssi?
	}
}

void nextBrightnessMode()
{
	setBrightnessMode((amulet_brightness_mode_t)(((int)g_brightness_mode + 1) % (int)AMULET_BRIGHTNESS_MODE_COUNT));
}
