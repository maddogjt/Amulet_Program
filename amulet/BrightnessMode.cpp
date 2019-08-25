#include "BrightnessMode.h"
#include "settings.h"
#include <FastLED.h>


amulet_brightness_mode_t g_brightness_mode = AMULET_BRIGHTNESS_MEDIUM;
bool g_override_brightness_mode = false;

void setBrightnessMode(amulet_brightness_mode_t mode)
{
	g_brightness_mode = mode;

	if (g_override_brightness_mode) {
		mode = AMULET_BRIGHTNESS_HIGH;
	}

	uint8_t newBrightness = 0;
	bool newLedPower = true;
	switch (mode)
	{
	case AMULET_BRIGHTNESS_LOW:
		newBrightness = localSettings_.brightness_[0];
		break;
	case AMULET_BRIGHTNESS_MEDIUM:
		newBrightness = localSettings_.brightness_[1];
		break;
	case AMULET_BRIGHTNESS_HIGH:
		newBrightness = localSettings_.brightness_[2];
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
		FastLED.setBrightness(newBrightness);
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

void refreshBrightness()
{
	setBrightnessMode(g_brightness_mode);
}

void overrideBrightnessMode(bool over) {
	g_override_brightness_mode = over;
	refreshBrightness();
}
