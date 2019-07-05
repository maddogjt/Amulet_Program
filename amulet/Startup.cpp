#include "Startup.h"

#include <Arduino.h>

#include "ble.h"
#include "led.h"
#include "animations.h"
#include "BrightnessMode.h"

amulet_mode_t mode = AMULET_MODE_STARTUP;

bool isAmulet()
{
	return mode == AMULET_MODE_AMULET || mode == AMULET_MODE_BEACON_POWER_AMULET || mode == AMULET_MODE_RUNIC_POWER_AMULET;
}

void startAsRemoteConfig(const StartupConfig &config);
void startAsAmulet(const StartupConfig &config);
void startAsBeacon(const StartupConfig &config);
void startAsRune(const StartupConfig &config);
void startAsPowerAmulet(const StartupConfig &config);

void startWithConfig(const StartupConfig &config)
{
	switch (config.mode)
	{
	case AMULET_MODE_STARTUP:
		startAsRemoteConfig(config);
		break;
	case AMULET_MODE_AMULET:
		startAsAmulet(config);
		break;
	case AMULET_MODE_BEACON:
		startAsBeacon(config);
		break;
	case AMULET_MODE_RUNE:
		startAsRune(config);
		break;
	case AMULET_MODE_BEACON_POWER_AMULET:
	case AMULET_MODE_RUNIC_POWER_AMULET:
		startAsPowerAmulet(config);
		break;

	default:
		startAsRemoteConfig(config);
		break;
	}
}

void start()
{
	StartupConfig config{};
	config.mode = AMULET_MODE_STARTUP;
	startWithConfig(config);
}

void startAsRemoteConfig(const StartupConfig &config)
{
	// digitalWrite(LED_BUILTIN, LED_STATE_ON);
	led_setup();
	ble_setup(false, true, true);

	// set master brightness control
	FastLED.setBrightness(g_led_brightness_medium);

	// Set the initial ambient animation
	led_set_ambient_animation(config.pattern);
}

void startAsAmulet(const StartupConfig &config)
{
	mode = AMULET_MODE_AMULET;
	led_setup();
	ble_setup(true, true);

	// set master brightness control
	FastLED.setBrightness(g_led_brightness_medium);

	// Set the initial ambient animation
	led_set_ambient_animation(config.pattern);
}

void startAsBeacon(const StartupConfig &config)
{
	mode = AMULET_MODE_BEACON;

	// Final config should be no LEDs but for now I'm turning on the LEDs and LED_BUILTIN
	digitalWrite(LED_BUILTIN, LED_STATE_ON);
	// digitalWrite(PIN_RGB_LED_PWR, !RGB_LED_PWR_ON);
	led_setup();
	FastLED.setBrightness(g_led_brightness_medium);

	ble_setup(true, true);

	led_set_ambient_animation(config.pattern);
	ble_set_advertisement_data(AdvertisementType::Beacon, config.ad, (uint8_t *)&config.pattern, sizeof(config.pattern));
}

void startAsRune(const StartupConfig &config)
{
	mode = AMULET_MODE_RUNE;

	// Final config should be no LEDs but for now I'm turning on the LEDs and LED_BUILTIN
	digitalWrite(LED_BUILTIN, LED_STATE_ON);
	// digitalWrite(PIN_RGB_LED_PWR, !RGB_LED_PWR_ON);
	led_setup();
	FastLED.setBrightness(g_led_brightness_medium);

	ble_setup(true, true);

	led_set_ambient_animation(config.pattern);
	ble_set_advertisement_data(AdvertisementType::Runic, config.ad, (uint8_t *)&config.pattern, sizeof(config.pattern));
}

void startAsPowerAmulet(const StartupConfig &config)
{
	mode = config.mode;
	led_setup();
	ble_setup(false, true, true);

	// set master brightness control
	FastLED.setBrightness(g_led_brightness_medium);

	// Set the initial ambient animation
	led_set_ambient_animation(config.pattern);
}
