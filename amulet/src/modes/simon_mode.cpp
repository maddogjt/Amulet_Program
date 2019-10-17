#include "simon_mode.h"
#include "../animation/animations.h"
#include "../communication/advertising.h"
#include "../communication/signal.h"
#include "../settings/settings.h"
#include "../leds/led.h"

void SimonMode::start()
{
	startNewSequence();
}

void SimonMode::startNewSequence()
{
	advertising_stop();

	simonSeed_ = rand();
	simonLevel_ = 1;

	anim_config_t anim {};
	anim.anim_ = Anim::AnimSimon;
	anim.extra0_ = simonSeed_;
	anim.extra1_ = simonLevel_;

	auto &config = localSettings_.startupConfig_;
	advertising_start(AdvertisementType::Beacon, config.ad,
					  (uint8_t *)&anim, sizeof(anim_config_t));
	start_animation_if_new(anim);
}

void SimonMode::loop()
{
	AmuletMode::loop();
}

void SimonMode::buttonPressMode()
{
	startNewSequence();
}