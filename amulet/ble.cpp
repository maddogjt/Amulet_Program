#include "ble.h"
#include "src/communication/parameters.h"
#include "src/communication/scanning.h"
#include "src/communication/advertising.h"

#include <bluefruit.h>

#include "settings.h"


void ble_setup(bool uart)
{
	// Initialize Bluefruit
	Bluefruit.autoConnLed(false); // Don't blink the led
	if (!Bluefruit.begin(1, 1))
	{
		// We need to start as both a peripheral and a central
		digitalWrite(LED_BUILTIN, !LED_STATE_ON);
		Serial.println("Unable to init bluetooth");
	}
	Bluefruit.setTxPower(globalSettings_.txPower_); // 4 is (probably) the strongest tx power that we can support.

	advertising_setup(true, uart);

	scanning_setup();
}
