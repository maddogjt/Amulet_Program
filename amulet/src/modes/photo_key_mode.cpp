#include "photo_key_mode.h"
#include "../animation/animations.h"
#include "../communication/advertising.h"
#include "../communication/signal.h"
#include "../settings/settings.h"
#include "../leds/led.h"

void PhotoKeyMode::start()
{
	// Set the initial ambient animation
	auto &config = localSettings_.startupConfig_;
	start_animation_if_new(config.photoKeyPattern_);
	advertising_start(AdvertisementType::Beacon, config.ad,
					  (uint8_t *)&config.photoKeyPattern_, sizeof(anim_config_t));
}

void PhotoKeyMode::loop()
{
}

void PhotoKeyMode::buttonPressMode()
{
	AmuletMode::buttonPressMode();
}