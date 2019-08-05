#include "ble.h"
#include "src/communication/parameters.h"
#include "src/communication/scanning.h"
#include "src/communication/advertising.h"

#include <bluefruit.h>

#include "settings.h"
#include "Startup.h"

static uint32_t powerActivatedTimestamp = 0;
bool powerIsAdvertising = false;

void ble_loop()
{
	EVERY_N_SECONDS(60)
	{
		// turn back on the blue light if we are in config mode and the user snoozed it.
		if (gMode == AMULET_MODE_CONFIG)
		{
			digitalWrite(LED_BUILTIN, LED_STATE_ON);
		}
	}

	if (powerIsAdvertising && gMode == AMULET_MODE_BEACON_POWER_AMULET)
	{
		const uint32_t powerAdvertisingWindow = 15000;
		if (millis() > powerActivatedTimestamp + powerAdvertisingWindow)
		{
			powerIsAdvertising = false;
			advertising_stop();
			animPattern ap = defaultConfigForRemoteSetup().pattern;
			advertising_start(AdvertisementType::Amulet, localSettings_.startupConfig_.ad, (uint8_t *)&ap, sizeof(ap));
		}
	}
}

void startPowerAmuletSuperpower()
{
	if (gMode != AMULET_MODE_BEACON_POWER_AMULET)
	{
		Serial.println("Need to be a power amulet to have superpowers");
		return;
	}
	powerIsAdvertising = true;
	powerActivatedTimestamp = millis();

	advertising_stop();

	animPattern power = localSettings_.powerPattern_;

	advertising_start(AdvertisementType::Beacon, localSettings_.startupConfig_.ad, (uint8_t *)&power, sizeof(power));
}

void ble_setup(bool advertise, bool scan, bool uart)
{
	// Initialize Bluefruit
	Bluefruit.autoConnLed(false); // Don't blink the led
	if (!Bluefruit.begin(advertise || uart, scan))
	{
		// We need to start as both a peripheral and a central
		digitalWrite(LED_BUILTIN, !LED_STATE_ON);
		Serial.println("Unable to init bluetooth");
	}
	Bluefruit.setTxPower(globalSettings_.txPower_); // 4 is (probably) the strongest tx power that we can support.

	advertising_setup(advertise, uart);

	if (scan)
	{
		scanning_setup();
	}
}
