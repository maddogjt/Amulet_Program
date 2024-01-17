#include "lamp_mode.h"
#include "../animation/animations.h"
#include "../communication/signal.h"
#include "../settings/settings.h"
#include "../leds/led.h"

#define MIMIC_IDX 14
#define RUNIC_IDX 15

anim_config_t kLampAnim{
	.anim_ = Anim::AnimSolid,
	.color1_ = 255,
	.color2_ = 255,
	.modifiers_ = AnimationModifier::None,
	.extra0_ = 255,
	.overlay_ = Anim::AnimSolid,
	.filter_ = OverlayFilter::None,
};

amulet_mode_t LampMode::previousMode_;

void LampMode::start() {
	LOG_LV1("MODE", "Lamp start");

	auto &config = localSettings_.startupConfig_;
	start_animation(kLampAnim);
	led_override_brightness_value(64);

	// Let's not advertise while in lamp
	advertising_stop();
}

void LampMode::loop() {

	// Check for incoming broadcast,
	// And adjust advertistment
	EVERY_N_MILLISECONDS(globalSettings_.animationUpdateTimer_)
	{

	}
}

void LampMode::restorePreviousMode()
{
	led_override_brightness_value(0);
	amulet_mode_start(previousMode_, localSettings_.startupConfig_.enterConfigMode_);
}

// Brightness/Power Action
void LampMode::buttonActionReset(button_action_t action)
{
	if (action == BUTTON_ACTION_PRESS)
	{
		resetPressed_ = true;
	}
	else if (resetPressed_ && action == BUTTON_ACTION_RELEASE)
	{
		restorePreviousMode();
	}
}

// Change Animation
void LampMode::buttonActionMode(button_action_t action) 
{
	if (action == BUTTON_ACTION_PRESS)
	{
		modePressed_ = true;
	}
	else if (modePressed_ && action == BUTTON_ACTION_RELEASE )
	{
		restorePreviousMode();
	}
}

void start_lamp_mode(amulet_mode_t returnMode)
{
	LampMode::previousMode_ = returnMode;

	amulet_mode_start(AMULET_MODE_LAMP, localSettings_.startupConfig_.enterConfigMode_);
}