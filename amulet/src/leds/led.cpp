#include "led.h"

#include "../settings/settings.h"

#include <FastLED.h>

#define DATA_PIN PIN_RGB_LEDS
#define EXTERNAL_LED_PIN 28
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB

CRGB gLeds[RGB_LED_COUNT];

CRGB externalLeds[EXTERNAL_LED_MAX_COUNT];
CLEDController *intLeds;
CLEDController *extLeds;

uint8_t intBrightness = 16;
uint8_t extBrightness = 16;

void led_setup()
{
#if !defined(NO_RGB_LEDS)
	pinMode(PIN_RGB_LED_PWR, OUTPUT);
	digitalWrite(PIN_RGB_LED_PWR, RGB_LED_PWR_ON);

	intLeds = &FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(gLeds, RGB_LED_COUNT).setCorrection(TypicalLEDStrip);
	extLeds = &FastLED.addLeds<LED_TYPE, EXTERNAL_LED_PIN, RGB>(externalLeds, EXTERNAL_LED_MAX_COUNT).setCorrection(TypicalLEDStrip);
#endif
}

void showSingle(CLEDController *pCur, uint8_t scale)
{
	uint8_t d = pCur->getDither();
	if (FastLED.getFPS() < 100)
	{
		pCur->setDither(0);
	}
	pCur->showLeds(scale);
	pCur->setDither(d);
}

void led_loop(int step)
{
#if !defined(NO_RGB_LEDS)
	memset(externalLeds, 0, sizeof(externalLeds));
	if (localSettings_.startupConfig_.externalLedEn_)
	{

		for (int i = 0; i < localSettings_.startupConfig_.externalLedCount_; i++)
		{
			if (!localSettings_.startupConfig_.externalLedExtend_)
			{
				externalLeds[i] = gLeds[i % RGB_LED_COUNT];
			}
			else
			{
				float pos = ((float)i) / localSettings_.startupConfig_.externalLedCount_;
				float orgPos = pos * RGB_LED_COUNT;
				int low = (int)floor(orgPos);
				float frac = orgPos - low;
				externalLeds[i] = gLeds[low].lerp8(gLeds[low + 1], frac * 255);
			}
		}
	}
	FastLED.waitShow();
	uint8_t scale = FastLED.getScale();
	showSingle(intLeds, intBrightness);
	showSingle(extLeds, extBrightness);
	FastLED.countFPS();
	// FastLED.show();
#endif
}

void customShow(uint8_t scale)
{
	// guard against showing too rapidly
	// while (m_nMinMicros && ((micros() - lastshow) < m_nMinMicros))
	// 	;
	// lastshow = micros();

	// // If we have a function for computing power, use it!
	// auto powerFunc = FastLED.getPowerFunc();
	// if (powerFunc)
	// {
	// 	scale = (*powerFunc)(scale, m_nPowerData);
	// }
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

	intBrightness = 0;
	extBrightness = 0;
	bool newLedPower = localSettings_.startupConfig_.intLedEn_;
	switch (brightness)
	{
	case LedBrightness::Low:
		intBrightness = localSettings_.startupConfig_.brightness_[0];
		extBrightness = localSettings_.startupConfig_.extBrightness_[0];
		break;
	case LedBrightness::Medium:
		intBrightness = localSettings_.startupConfig_.brightness_[1];
		extBrightness = localSettings_.startupConfig_.extBrightness_[1];
		break;
	case LedBrightness::High:
		intBrightness = localSettings_.startupConfig_.brightness_[2];
		extBrightness = localSettings_.startupConfig_.extBrightness_[2];
		break;
	case LedBrightness::Off:
	case LedBrightness::Count:
	default:
		newLedPower = false;
	}

#if !defined(NO_RGB_LEDS)

	// if (newLedPower)
	// {
	// Turn on LED power rail
	LOG_LV1("BRIT", "Setting brightness to %d (mode %d)", intBrightness, brightness);
	digitalWrite(PIN_RGB_LED_PWR, newLedPower ? RGB_LED_PWR_ON : !RGB_LED_PWR_ON);
	FastLED.setBrightness(intBrightness);
	// }
	// else
	// {
	// LOG_LV1("BRIT", "Turning off LED power rail (mode %d)", brightness);
	// FastLED.setBrightness(newBrightness);
	// Turn off the LED power rail
	// digitalWrite(PIN_RGB_LED_PWR, !RGB_LED_PWR_ON);

	// TODO: Also turn off bluetooth.
	// FEATURE: Maybe advertise in special off mode so we can find lost beacons by rssi?
	// }
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
	g_override_brightness_mode = over;
	led_refresh_brightness();
}
