#include "Startup.h"

#include <Arduino.h>

#include "ble.h"
#include "led.h"
#include "animations.h"
#include "BrightnessMode.h"

amulet_mode_t mode = AMULET_MODE_STARTUP;

void startAsRemoteConfig();
void startAsAmulet();
void startAsBeacon();
void startAsRune();
void startAsPowerAmulet();

void start()
{
	// Check for previous configuration

	// If no previous configuration, go to uart mode
	// advertise as BLEUart
	// If no connect after 60 seconds , start as an amulet with random rune
	// startAsAmulet();
	// startAsBeacon();
	startAsPowerAmulet();
}

void startAsRemoteConfig()
{
	digitalWrite(LED_BUILTIN, LED_STATE_ON);
	ble_setup_uart();
}

void startAsAmulet()
{
	mode = AMULET_MODE_AMULET;
	led_setup();
	ble_setup(true, true);

	// set master brightness control
	FastLED.setBrightness(g_led_brightness_medium);

	// Set the initial ambient animation
	led_set_ambient_animation({.name = Anim::AnimRainbowRaster,
							   .params = {}});
}

void startAsBeacon()
{
	mode = AMULET_MODE_BEACON;

	// Final config should be no LEDs but for now I'm turning on the LEDs and LED_BUILTIN
	digitalWrite(LED_BUILTIN, LED_STATE_ON);
	// digitalWrite(PIN_RGB_LED_PWR, !RGB_LED_PWR_ON);
	led_setup();
	FastLED.setBrightness(g_led_brightness_medium);

	ble_setup(true, true);

	animPattern beaconPattern = {.name = Anim::AnimBallRaster,
								 .params = {}};

	// Anim name is 4 bytes - could shrink
	LOG_LV1("STRT", "size of name: %d", sizeof(Anim::AnimCylon));
	led_set_ambient_animation(beaconPattern);
	ble_set_advertisement_data(AdvertisementType::Beacon, (uint8_t *)&beaconPattern, sizeof(beaconPattern));
}

void startAsRune()
{
	mode = AMULET_MODE_RUNE;

	// Final config should be no LEDs but for now I'm turning on the LEDs and LED_BUILTIN
	digitalWrite(LED_BUILTIN, LED_STATE_ON);
	// digitalWrite(PIN_RGB_LED_PWR, !RGB_LED_PWR_ON);
	led_setup();
	FastLED.setBrightness(g_led_brightness_medium);

	ble_setup(true, true);

	animPattern runePattern = {.name = Anim::AnimBallRaster,
							   .params = {}};

	// Anim name is 4 bytes - could shrink
	LOG_LV1("STRT", "size of name: %d", sizeof(Anim::AnimBPM));
	led_set_ambient_animation(runePattern);
	ble_set_advertisement_data(AdvertisementType::Rune, (uint8_t *)&runePattern, sizeof(runePattern));
}

void startAsPowerAmulet()
{
	mode = AMULET_MODE_POWER_AMULET;
	led_setup();
	ble_setup(true, true);

	// set master brightness control
	FastLED.setBrightness(g_led_brightness_medium);

	// Set the initial ambient animation
	led_set_ambient_animation({.name = Anim::AnimRainbowRaster,
							   .params = {}});
}
