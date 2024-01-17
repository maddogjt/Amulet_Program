#include "rune_mode.h"
#include "../animation/animations.h"
#include "../communication/signal.h"
#include "../settings/settings.h"
#include "../leds/led.h"

void RuneMode::start() {
	LOG_LV1("MODE", "Rune start");

	// When waking from power off, the button used to wake should not trigger an action.
	ignoreNextButton = true;

	auto &config = localSettings_.startupConfig_;
	auto &pattern = config.burnPattern_;
	start_animation(pattern);
	advertising_start(AdvertisementType::Runic, config.ad, 
		(uint8_t *)&pattern, sizeof(anim_config_t));
}

void RuneMode::loop() {

	EVERY_N_MILLISECONDS(globalSettings_.animationUpdateTimer_)
	{
		if (brightnessChanged_) {
			brightnessChanged_ = false;
			if (led_get_brightness() == LedBrightness::Off ) {
				digitalWrite(LED_BUILTIN, LED_STATE_ON);
			} else {
				digitalWrite(LED_BUILTIN, !LED_STATE_ON);
			}
		}
	}
}

void RuneMode::ledBrightnessAction() {
	led_next_brightness();
	brightnessChanged_ = true;
}

// Brightness/Power Action
void RuneMode::buttonActionReset(button_action_t action)
{
	if (action != BUTTON_ACTION_RELEASE) {
		return;
	}
	LOG_LV1("MODE", "Rune press reset");
	if (!ignoreNextButton) {
		this->ledBrightnessAction();
	} else {
		ignoreNextButton = false;
	}
}

// Change Animation
void RuneMode::buttonActionMode(button_action_t action) {
	LOG_LV1("MODE", "Rune press mode");
	if (action != BUTTON_ACTION_RELEASE)
	{
		return;
	}

	if (!ignoreNextButton) {
		this->ledBrightnessAction();
	} else {
		ignoreNextButton = false;
	}
}
