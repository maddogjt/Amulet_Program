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
	sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);
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

	if (localSettings_.startupConfig_.mode==AMULET_MODE_FIRSTBOOT) {
		run_first_boot();
	}

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

bool dfuPrevPressedFor2k = false;
bool resetPrevPressedFor2k = false;

void loop()
{
	// LOG_LV2("LOOP", "Loop start");
	led_loop(step);
	signal_loop(step);
	ble_loop();

	dfuButton.read();
	if (!devEnabled)
	{
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

	if (dfuButton.pressedFor(5000))
	{
		auto start = millis();
		Serial.println("Going to sleep soon");

		digitalWrite(LED_BUILTIN, LED_STATE_ON);
		delay(500);

		digitalWrite(PIN_RGB_LED_PWR, !RGB_LED_PWR_ON);
		digitalWrite(LED_BUILTIN, !LED_STATE_ON);

		while (millis() - start < 10000)
		{
			dfuButton.read();
			// User released DFU button, before 10 seconds, so don't go to programming mode.
			if (dfuButton.isReleased())
			{
				power_off();
			}
			FastLED.delay(5);
		}

		// Turn on blue LED to signal timer tripped and let user should release button
		digitalWrite(LED_BUILTIN, LED_STATE_ON);
		while (dfuButton.isPressed())
		{
			FastLED.delay(5);
			dfuButton.read();
		}

		// Didn't relase mode, guess it's time to go to config mode (or reset to amulet mode if already in config)
		if (localSettings_.startupConfig_.mode == AMULET_MODE_CONFIG)
		{
			localSettings_.startupConfig_.mode = AMULET_MODE_RUNE;
		}
		else
		{
			localSettings_.startupConfig_.mode = AMULET_MODE_CONFIG;
		}

		write_local_settings();

		uint8_t sd_en = 0;
		(void) sd_softdevice_is_enabled(&sd_en);

		if ( sd_en )
		{
			sd_nvic_SystemReset();
		}else
		{
			NVIC_SystemReset();
		}
	}

	if (dfuButton.wasReleased() || (!devEnabled && resetButton.wasReleased()))
	{
		if ((dfuButton.wasReleased() && dfuPrevPressedFor2k) ||
			(!devEnabled && resetButton.wasReleased() && resetPrevPressedFor2k))
		{
			Serial.println("Possibly trigger superpower here");
			startPowerAmuletSuperpower();
		}
		else
		{
			nextBrightnessMode();
		}
	}

	dfuPrevPressedFor2k = dfuButton.pressedFor(2000);
	resetPrevPressedFor2k = (!devEnabled) ? resetButton.pressedFor(2000) : false;
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

	digitalWrite(LED_BUILTIN, LED_STATE_ON);
	delay(500);

	digitalWrite(PIN_RGB_LED_PWR, !RGB_LED_PWR_ON);
	digitalWrite(LED_BUILTIN, !LED_STATE_ON);

	power_off();
}


void run_first_boot() {
	led_setup();
	FastLED.setBrightness(20);
	for (auto i =0; i < 10; i++) {
		gLeds[i] = CRGB::Red;
	}
	FastLED.show();
	delay(1000);

	for (auto i =0; i < 10; i++) {
		gLeds[i] = CRGB::Green;
	}
	FastLED.show();
	delay(1000);

		for (auto i =0; i < 10; i++) {
		gLeds[i] = CRGB::Blue;
	}
	FastLED.show();
	delay(1000);

	localSettings_.startupConfig_.mode = AMULET_MODE_AMULET;
	write_local_settings();

	
	systemSleep();
}

void power_off()
{
#define DFU_MAGIC_IGNORE_PIN			0xC6


    nrf_gpio_cfg_sense_input(PIN_DFU, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
	nrf_gpio_cfg_sense_input(PIN_RESET, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);

	NRF_POWER->GPREGRET = DFU_MAGIC_IGNORE_PIN;
    NRF_POWER->SYSTEMOFF = 1;
}