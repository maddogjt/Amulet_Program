#include "config_mode.h"
#include "../../led.h"
#include "../../settings.h"
#include "../animation/animations.h"

void ConfigMode::start()
{
	Serial.println("Start config mode");

	// Turn the light hard on to indicate config mode
	digitalWrite(LED_BUILTIN, LED_STATE_ON);

	auto &config = localSettings_.startupConfig_;

	// Set the initial ambient animation
	start_animation(config.getConfigModeAnim(config.mode));
}

void ConfigMode::loop() {
	digitalWrite(LED_BUILTIN, LED_STATE_ON);
}
