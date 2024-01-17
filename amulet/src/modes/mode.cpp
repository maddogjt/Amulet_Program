#include "mode.h"
#include "blinky_mode.h"
#include "rune_mode.h"
#include "lamp_mode.h"
#include "config_mode.h"

#include "../leds/led.h"

AmuletMode *gAmuletMode = nullptr;

const char *get_config_mode_name(amulet_mode_t mode)
{
	switch (mode)
	{
	case AMULET_MODE_BLINKY:
		return "blinky";
		break;
	case AMULET_MODE_RUNE:
		return "rune";
		break;
	case AMULET_MODE_BEACON:
		return "beacon";
		break;
	case AMULET_MODE_LAMP:
		return "lamp";
		break;
	case AMULET_MODE_COUNT:
	default:
		break;
	}
	return "err";
}

AmuletMode *amulet_mode_start(amulet_mode_t mode, bool enterConfig)
{
	if (gAmuletMode != nullptr) {
		delete gAmuletMode;
		gAmuletMode = nullptr;
	}

	if (enterConfig) {
		gAmuletMode = new ConfigMode();
	} else if (mode == AMULET_MODE_BLINKY) {
		gAmuletMode = new BlinkyMode();
	} else if (mode == AMULET_MODE_RUNE) {
		gAmuletMode = new RuneMode();
	} else if (mode == AMULET_MODE_BEACON) {
		gAmuletMode = new RuneMode();
	} else if (mode == AMULET_MODE_LAMP) {
		gAmuletMode = new LampMode();
	} else
	{
		gAmuletMode = new ConfigMode();
	}

	gAmuletMode->start();
	return gAmuletMode;
}

AmuletMode *amulet_mode_get()
{
	return gAmuletMode;
}

void amulet_mode_loop() {
	if (gAmuletMode != nullptr) {
		gAmuletMode->loop();
	}
}

void AmuletMode::loop() {

}

void AmuletMode::buttonActionMode(button_action_t action) {
	if (action == BUTTON_ACTION_RELEASE) {
		led_next_brightness();
	}
}

void AmuletMode::buttonActionReset(button_action_t action)
{
	if (action == BUTTON_ACTION_RELEASE) {
		led_next_brightness();
	}
}