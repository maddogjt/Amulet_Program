#include "mode.h"
#include "burn_mode.h"
#include "simon_mode.h"
#include "seacomp_amulet_mode.h"
#include "config_mode.h"

#include "../leds/led.h"

AmuletMode *gAmuletMode = nullptr;

const char *get_config_mode_name(amulet_mode_t mode)
{
	switch (mode)
	{
	case AMULET_MODE_BURN:
		return "burn";
		break;
	case AMULET_MODE_SIMON:
		return "simon";
		break;
	case AMULET_MODE_SEACOMP_AMULET:
		return "seacompam";
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
		gAmuletMode->start();
	} else if (mode == AMULET_MODE_BURN) {
		gAmuletMode = new BurnMode();
		gAmuletMode->start();
	}
	else if (mode == AMULET_MODE_SIMON)
	{
		gAmuletMode = new SimonMode();
		gAmuletMode->start();
	}
	else if (mode == AMULET_MODE_SEACOMP_AMULET)
	{
		gAmuletMode = new SeacompAmuletMode();
		gAmuletMode->start();
	}
	else
	{
		gAmuletMode = new ConfigMode();
		gAmuletMode->start();
	}

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

void AmuletMode::buttonPressMode() {
	led_next_brightness();
}

void AmuletMode::buttonPressReset()
{
	led_next_brightness();
}