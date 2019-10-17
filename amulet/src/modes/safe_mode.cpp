#include "safe_mode.h"
#include "../animation/animations.h"
#include "../communication/advertising.h"
#include "../communication/signal.h"
#include "../settings/settings.h"
#include "../leds/led.h"

void SafeMode::start() {
	// Set the initial ambient animation
	auto &config = localSettings_.startupConfig_;
	start_animation_if_new(config.safePattern_);
	advertising_start(AdvertisementType::Beacon, config.ad, 
		(uint8_t *)&config.safePattern_, sizeof(anim_config_t));
}

void SafeMode::loop() {
}

void SafeMode::buttonPressMode() {
	AmuletMode::buttonPressMode();
}