#include <JC_Button.h>

#include "animations/animation.h"
#include "src/animation/animations.h"
#include "src/communication/ble.h"
#include "src/communication/signal.h"
#include "src/leds/led.h"
#include "src/settings/dev_mode.h"
#include "src/settings/settings.h"
#include "src/misc/system_sleep.h"

Button modeButton(PIN_DFU, 25, true, true);
Button resetButton(PIN_RESET, 25, true, true);

bool devEnabled = false;

void setup()
{
#if defined(USE_DCDC)
	sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);
#endif
	// Set pin modes
	pinMode(LED_BUILTIN, OUTPUT);

	// Start Serial
	Serial.begin(115200);
	Serial.println("Amulet startup");

	// init settings
	settings_init();

	if (localSettings_.startupConfig_.mode==AMULET_MODE_FIRSTBOOT) {
		run_first_boot();
	}

	// Start reading the Mode button so we can trigger off long and short presses
	modeButton.begin();

	devEnabled = dev_mode_enabled();
	if (!devEnabled)
	{
		resetButton.begin();
	}

	auto &config = localSettings_.startupConfig_;

	led_setup();

	// set master brightness control
	led_set_brightness(LedBrightness::High);

	bool enableUart = config.enterConfigMode_;
	ble_setup(enableUart);


	amulet_mode_start(config.mode, config.enterConfigMode_);
}

int step = 0;

bool longPressMode = false;
bool longPressReset = false;
int modeLastReleased = 0;

void loop()
{
	// LOG_LV2("LOOP", "Loop start");
	signal_loop(step);
	amulet_mode_loop();
	// Step the currentAnimation
	step_animation();
	led_loop(step);

	modeButton.read();

	// If the board is not already in devmode, poll the reset button.
	if (!devEnabled)
	{
		resetButton.read();

		// Detect the combo for entering devmode
		// Hold reset for at least 10 seconds then double press mode
		if (resetButton.pressedFor(10000) && modeButton.wasReleased())
		{
			if (millis() - modeLastReleased < 500)
			{

				Serial.println("Toggling dev mode");
				set_dev_mode_enabled_and_reboot();
				// Long press means power down.
				//systemSleep();
			}
		}
	}

	// If mode is pressed for more than 5 seconds, begin shutdown (possibly allow programming toggle)
	if (modeButton.pressedFor(5000))
	{
		auto sleepSequenceStart = millis();
		Serial.println("Going to sleep soon");

		// Pulse the blue builtin LED for half a seconds
		digitalWrite(LED_BUILTIN, LED_STATE_ON);
		delay(500);
		digitalWrite(LED_BUILTIN, !LED_STATE_ON);

		// And turn off the LEDs
		led_set_brightness(LedBrightness::Off);

		// Wait for mode button to be released before powering off.  If the reset button is
		// pressed before 
		while (true)
		{
			modeButton.read();
			// User released Mode button, before 10 seconds, so don't go to programming mode.
			if (modeButton.isReleased())
			{
				power_off();
			}

			// if reset pressed, break loop and go to config
			resetButton.read();
			if (resetButton.wasPressed()) {
				break;
			}

			FastLED.delay(10);
		}

		// Turn on blue LED to signal that we're entering config mode
		digitalWrite(LED_BUILTIN, LED_STATE_ON);

		// Toggle the config mode state & update the settings
		localSettings_.startupConfig_.enterConfigMode_ = !localSettings_.startupConfig_.enterConfigMode_;
		write_local_settings();

		// Reboot the device in the new mode
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

	// Was the button just pressed
	if (modeButton.wasPressed())
	{
		amulet_mode_get()->buttonActionMode(BUTTON_ACTION_PRESS);
	}

	if (!longPressMode && modeButton.pressedFor(1000))
	{
		amulet_mode_get()->buttonActionMode(BUTTON_ACTION_LONG_PRESS_START);
	}

	if (modeButton.wasReleased())
	{
		modeLastReleased = millis();
		auto action = longPressMode ? BUTTON_ACTION_LONG_PRESS_RELEASE : BUTTON_ACTION_RELEASE;
		amulet_mode_get()->buttonActionMode(action);
	}

	longPressMode = modeButton.pressedFor(1000);

	if (!devEnabled)
	{
		if (resetButton.wasPressed())
		{
			amulet_mode_get()->buttonActionReset(BUTTON_ACTION_PRESS);
		}

		if (!longPressReset && resetButton.pressedFor(1000)) {
			amulet_mode_get()->buttonActionReset(BUTTON_ACTION_LONG_PRESS_START);
		}

		if (resetButton.wasReleased())
		{
			auto action = longPressReset ? BUTTON_ACTION_LONG_PRESS_RELEASE : BUTTON_ACTION_RELEASE;
			amulet_mode_get()->buttonActionReset(action);
		}

		longPressReset = resetButton.pressedFor(1000);

		if (resetButton.wasPressed())
		{
			resetLastPressed = millis();
		}
		if (resetButton.wasReleased())
		{
			resetLastReleased = millis();
		}
	}

	FastLED.delay(1000 / ANIMATION_FRAMERATE);
	step++;
}

extern CRGB gLeds[RGB_LED_COUNT];

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

	localSettings_.startupConfig_.mode = AMULET_MODE_BLINKY;
	write_local_settings();

	
	systemSleep();
}

