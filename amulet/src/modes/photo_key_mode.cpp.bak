#include "photo_key_mode.h"
#include "../animation/animations.h"
#include "../communication/advertising.h"
#include "../communication/signal.h"
#include "../settings/settings.h"
#include "../leds/led.h"
#include "../misc/seacomp_game.h"

void PhotoKeyMode::start()
{
	Serial.println("Starting Photo Key Mode");
	pinMode(PIN_A7, INPUT);
	startPuzzle();
}

void PhotoKeyMode::loop()
{
	int brightness = analogRead(PIN_A7);
	EVERY_N_MILLISECONDS(333)
	{
		Serial.println(brightness); // debug value
	}

	if (brightness > 100)
	{
		startVictory();
	}
}

void PhotoKeyMode::buttonPressMode()
{
	AmuletMode::buttonPressMode();
}

void PhotoKeyMode::startPuzzle()
{
	Serial.println("Starting Photo Key Puzzle");

	anim_config_t anim{};
	anim.anim_ = Anim::AnimConfetti;
	start_animation_if_new(anim);

	auto &config = localSettings_.startupConfig_;
	advertising_start(AdvertisementType::Beacon, config.ad, (uint8_t *)&anim, sizeof(anim_config_t));
}

void PhotoKeyMode::startVictory()
{
	Serial.println("Starting Photo Key Victory");

	advertising_stop();
	delay(100);

	// Start sending victory
	anim_config_t anim{};
	anim.anim_ = Anim::AnimVictory;
	anim.extra0_ = kSeacompGamePhotokey;
	start_animation_if_new(anim);
	auto &config = localSettings_.startupConfig_;

	advertising_start(AdvertisementType::Beacon, config.ad, (uint8_t *)&anim, sizeof(anim_config_t));
}
