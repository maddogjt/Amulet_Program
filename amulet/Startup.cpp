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
	startAsAmulet();

	// startAsBeacon();
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
	led_set_ambient_animation({.name = Anim::AnimBallRaster,
							   .params = {}});
}

void startAsBeacon()
{
	mode = AMULET_MODE_BEACON;
	digitalWrite(PIN_RGB_LED_PWR, !RGB_LED_PWR_ON);
	ble_setup(true, false);
}

void startAsRune()
{
	mode = AMULET_MODE_RUNE;
	led_setup();
	ble_setup(true, true);
}

void startAsPowerAmulet()
{
	mode = AMULET_MODE_POWER_AMULET;
	led_setup();
	ble_setup(true, true);
}
