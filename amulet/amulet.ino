#include <JC_Button.h>
#define ARDUINO_GENERIC
#include <FastLED.h>
#undef ARDUINO_GENERIC

#include "globals.h"
#include "led.h"
#include "ble.h"
#include "signal.h"
#include "BrightnessMode.h"
#include "Startup.h"
#include "dev_mode.h"
#include "settings.h"

Button dfuButton(PIN_DFU, 25, true, true);
Button resetButton(PIN_RESET, 25, true, true);

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

void nextMode();
void systemSleep();

bool devEnabled = false;

void setup()
{
	// Set pin modes
	pinMode(LED_BUILTIN, OUTPUT);

	// Start Serial
	Serial.begin(115200);
	Serial.println("Amulet startup");

	// init settings
	settings_init();

	// Turn on LED power rail
	pinMode(PIN_RGB_LED_PWR, OUTPUT);
	digitalWrite(PIN_RGB_LED_PWR, RGB_LED_PWR_ON);

	// Start reading the DFU button so we can trigger off long and short presses
	dfuButton.begin();

	devEnabled = dev_mode_enabled();
	if (!devEnabled)
	{
		resetButton.begin();
	}

	start();
}

int step = 0;

void loop()
{
	// LOG_LV2("LOOP", "Loop start");
	led_loop(step);
	signal_loop(step);

	dfuButton.read();
	if (!devEnabled) {
		resetButton.read();

		static int dfuLastReleased = 0;
		if (resetButton.pressedFor(10000) && dfuButton.wasReleased())
		{
			if (millis() - dfuLastReleased < 500)
			{

				Serial.println("Toggling dev mode");
				set_dev_mode_enabled_and_reboot();
				// Long press means power down.
				//systemSleep();
			}
			dfuLastReleased = millis();
		}
	}

	if (dfuButton.pressedFor(5000)) {
		systemSleep();
	}
	if (dfuButton.wasReleased() || (!devEnabled && resetButton.wasReleased()))
	{
		nextBrightnessMode();
	}

	FastLED.delay(1000 / FRAMERATE);
	step++;
}

/* 
void nextDebugModeButton()
{
	switch (mode)
	{
	case AMULET_MODE_AMULET:
		nextMode();
		break;
	case AMULET_MODE_BEACON:
		nextMode();
		break;
	case AMULET_MODE_RUNE:
		nextMode();
		break;
	case AMULET_MODE_LED_TEST:
		// Instead of going to the next mode, go to the next test pattern if there is one.
		if (led_test_has_next_pattern())
		{
			led_show_next_test_pattern();
		}
		else
		{
			led_show_test_pattern(false);
			nextMode();
		}
		break;
	case AMULET_MODE_COUNT:
	default:
		break;
	}
}

void nextMode()
{
	mode = (amulet_mode_t)(((int)mode + 1) % (int)AMULET_MODE_COUNT);
	setMode(mode);
}

void setMode(amulet_mode_t mode)
{
	switch (mode)
	{
	case AMULET_MODE_AMULET:
	case AMULET_MODE_BEACON:
	case AMULET_MODE_RUNE:
		break;
	case AMULET_MODE_LED_TEST:
		digitalWrite(LED_BUILTIN, !LED_STATE_ON);
		led_show_test_pattern(true);
		break;
	case AMULET_MODE_COUNT:
		// shouldn't be here
		break;
	}

	// Turn on the builtin if we are not an amulet.
	// digitalWrite(LED_BUILTIN, mode == AMULET_MODE_AMULET ? !LED_STATE_ON : LED_STATE_ON);
}
*/
void systemSleep()
{
	Serial.println("Going to sleep now");

	digitalWrite(PIN_RGB_LED_PWR, !RGB_LED_PWR_ON);
	digitalWrite(LED_BUILTIN, !LED_STATE_ON);

	systemOff(21, 0);
}
