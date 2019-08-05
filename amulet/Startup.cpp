#include "Startup.h"
#include "settings.h"

#include "src/communication/advertising.h"

#include <Arduino.h>

#include "ble.h"
#include "led.h"
#include "animations.h"
#include "BrightnessMode.h"

amulet_mode_t gMode = AMULET_MODE_AMULET;

bool isPowerAmulet()
{
	return gMode == AMULET_MODE_BEACON_POWER_AMULET;
}

bool isAmulet()
{
	return gMode == AMULET_MODE_AMULET || isPowerAmulet();
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
	case AMULET_MODE_CONFIG:
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
		startAsPowerAmulet(config);
		break;
	default:
		startAsRemoteConfig(config);
		break;
	}
}

void start()
{

	// char buffer[200];
	// serializeAnimPattern(buffer, 200, config.pattern);
	// testSerialization();
	// StartupConfig config{};
	// startAsRemoteConfig(config);

	startWithConfig(localSettings_.startupConfig_);
}

void startAsRemoteConfig(const StartupConfig &config)
{
	Serial.println("Start config mode");
	gMode = AMULET_MODE_CONFIG;

	// Turn the light hard on to indicate config mode
	digitalWrite(LED_BUILTIN, LED_STATE_ON);
	led_setup();
	ble_setup(false, true, true);

	// set master brightness control
	FastLED.setBrightness(g_led_brightness_medium);

	// Set the initial ambient animation
	led_set_ambient_animation(config.pattern);
}

void startAsAmulet(const StartupConfig &config)
{
	Serial.println("Start amulet mode");
	gMode = AMULET_MODE_AMULET;
	led_setup();
	ble_setup(true, true);

	// set master brightness control
	FastLED.setBrightness(g_led_brightness_medium);

	// Set the initial ambient animation
	led_set_ambient_animation(config.pattern);
	advertising_start(AdvertisementType::Amulet, config.ad, (uint8_t *)&config.pattern, sizeof(config.pattern));
}

void startAsBeacon(const StartupConfig &config)
{
	Serial.println("Start beacon mode");
	gMode = AMULET_MODE_BEACON;

	// Final config should be no LEDs but for now I'm turning on the LEDs and LED_BUILTIN
	digitalWrite(LED_BUILTIN, LED_STATE_ON);
	digitalWrite(PIN_RGB_LED_PWR, !RGB_LED_PWR_ON);
	led_setup();
	FastLED.setBrightness(g_led_brightness_medium);

	ble_setup(true, true);

	led_set_ambient_animation(config.pattern);
	advertising_start(AdvertisementType::Beacon, config.ad, (uint8_t *)&config.pattern, sizeof(config.pattern));
}

void startAsRune(const StartupConfig &config)
{
	Serial.println("Start rune mode");
	gMode = AMULET_MODE_RUNE;

	// Final config should be no LEDs but for now I'm turning on the LEDs and LED_BUILTIN
	// digitalWrite(LED_BUILTIN, LED_STATE_ON);
	// digitalWrite(PIN_RGB_LED_PWR, !RGB_LED_PWR_ON);
	led_setup();
	FastLED.setBrightness(g_led_brightness_medium);

	ble_setup(true, true);

	led_set_ambient_animation(config.pattern);
	advertising_start(AdvertisementType::Runic, config.ad, (uint8_t *)&config.pattern, sizeof(config.pattern));
}

void startAsPowerAmulet(const StartupConfig &config)
{
	Serial.printf("Start power amulet mode (%)\n", get_config_mode_name(config.mode));
	gMode = config.mode;

	led_setup();
	ble_setup(true, true);

	// set master brightness control
	FastLED.setBrightness(g_led_brightness_medium);

	// Set the initial ambient animation
	led_set_ambient_animation(config.pattern);
	advertising_start(AdvertisementType::Amulet, config.ad, (uint8_t *)&config.pattern, sizeof(config.pattern));
}
