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

void startAsRemoteConfig(StartupConfig &config);
void startAsAmulet(const StartupConfig &config);
void startAsBeacon(const StartupConfig &config);
void startAsRune(const StartupConfig &config);
void startAsPowerAmulet(const StartupConfig &config);

void startWithConfig(StartupConfig &config)
{
	if (config.enterConfigMode_) {
		startAsRemoteConfig(config);
		return;
	}

	switch (config.mode)
	{
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

void startAsRemoteConfig(StartupConfig &config)
{
	Serial.println("Start config mode");
	gMode = config.mode;

	// Turn the light hard on to indicate config mode
	digitalWrite(LED_BUILTIN, LED_STATE_ON);
	led_setup();
	ble_setup(false, true, true);

	// set master brightness control
	setBrightnessMode(AMULET_BRIGHTNESS_LOW);

	// Set the initial ambient animation
	led_set_ambient_animation(config.getConfigModeAnim(config.mode));
}

void startAsAmulet(const StartupConfig &config)
{
	Serial.println("Start amulet mode");
	gMode = AMULET_MODE_AMULET;
	led_setup();
	ble_setup(true, true);

	// set master brightness control
	setBrightnessMode(AMULET_BRIGHTNESS_LOW);

	// Set the initial ambient animation
	led_set_ambient_animation(config.ambientPattern_);
	advertising_start(AdvertisementType::Amulet, config.ad, (uint8_t *)&config.ambientPattern_, sizeof(config.ambientPattern_));
}

void startAsBeacon(const StartupConfig &config)
{
	Serial.println("Start beacon mode");
	gMode = AMULET_MODE_BEACON;

	// Final config should be no LEDs but for now I'm turning on the LEDs and LED_BUILTIN
	digitalWrite(LED_BUILTIN, LED_STATE_ON);
	digitalWrite(PIN_RGB_LED_PWR, !RGB_LED_PWR_ON);
	led_setup();
	setBrightnessMode(AMULET_BRIGHTNESS_LOW);

	ble_setup(true, true);

	led_set_ambient_animation(config.beaconPattern_);
	advertising_start(AdvertisementType::Beacon, config.ad, (uint8_t *)&config.beaconPattern_, sizeof(config.beaconPattern_));
}

void startAsRune(const StartupConfig &config)
{
	Serial.println("Start rune mode");
	gMode = AMULET_MODE_RUNE;

	// Final config should be no LEDs but for now I'm turning on the LEDs and LED_BUILTIN
	// digitalWrite(LED_BUILTIN, LED_STATE_ON);
	// digitalWrite(PIN_RGB_LED_PWR, !RGB_LED_PWR_ON);
	led_setup();
	setBrightnessMode(AMULET_BRIGHTNESS_LOW);


	ble_setup(true, true);

	led_set_ambient_animation(config.runePattern_);
	advertising_start(AdvertisementType::Runic, config.ad, (uint8_t *)&config.runePattern_, sizeof(config.runePattern_));
}

void startAsPowerAmulet(const StartupConfig &config)
{
	Serial.printf("Start power amulet mode (%)\n", get_config_mode_name(config.mode));
	gMode = config.mode;

	led_setup();
	ble_setup(true, true);

	// set master brightness control
	setBrightnessMode(AMULET_BRIGHTNESS_LOW);

	// Set the initial ambient animation
	led_set_ambient_animation(config.ambientPattern_);
	advertising_start(AdvertisementType::Amulet, config.ad, (uint8_t *)&config.ambientPattern_, sizeof(config.ambientPattern_));
}
