#include "seacomp_amulet_mode.h"
#include "../animation/animations.h"
#include "../communication/advertising.h"
#include "../communication/signal.h"
#include "../settings/settings.h"
#include "../leds/led.h"

void SeacompAmuletMode::start()
{

	for (int i = 0; i < (int)Anim::Count; i++) {
		animations_[i] = {};
		animations_[i].anim_ = (Anim)(i);
		animations_[i].speed_ = 4;

	}
	// Set the initial ambient animation
	auto &config = localSettings_.startupConfig_;
	start_animation(animations_[currentAnim_]);
	advertising_start(AdvertisementType::Amulet, config.ad, 
		(uint8_t *)&animations_[currentAnim_], sizeof(anim_config_t));
}

// returns true if animation was changed
void SeacompAmuletMode::set_animation_from_signal(Signal *s)
{
	if (s != nullptr)
	{
		anim_config_t pattern;
		VERIFY_STATIC(sizeof(pattern) <= kMaxPayloadLen);
		memcpy(&pattern, s->data_.payload, sizeof(anim_config_t));
		start_animation_if_new(pattern);
	}
	else
	{
		start_animation_if_new(animations_[currentAnim_]);
	}
}

void SeacompAmuletMode::loop()
{
	if (sendingPattern_ && millis() > sendPatternStop_)
	{
		//digitalWrite(LED_BUILTIN, !LED_STATE_ON);
		sendingPattern_ = false;
		advertising_stop();

		anim_config_t ap = localSettings_.startupConfig_.seacompAmuletPattern_;
		advertising_start(
			AdvertisementType::Amulet,
			localSettings_.startupConfig_.ad,
			(uint8_t *)&animations_[currentAnim_],
			sizeof(animations_[currentAnim_]));
		start_animation_if_new(animations_[currentAnim_]);
	}

	EVERY_N_MILLISECONDS(globalSettings_.animationUpdateTimer_)
	{
		led_override_brightness(false);
		if (!sendingPattern_)
		{
			Signal *signal = signal_get_current_top();
			set_animation_from_signal(signal);
			if (localSettings_.bikeMode_ && signal)
			{
				led_override_brightness(true);
			}
		}
	}
}

void SeacompAmuletMode::buttonHoldReset()
{
	start_animation_if_new(animations_[currentAnim_]);

	sendingPattern_ = true;
	sendPatternStop_ = millis() + 15000;

	advertising_stop();

	//digitalWrite(LED_BUILTIN, LED_STATE_ON);

	auto anim = animations_[currentAnim_];

	advertising_start(AdvertisementType::Beacon,
		localSettings_.startupConfig_.ad,
		(uint8_t *)&anim, 
		sizeof(anim));
}

void SeacompAmuletMode::buttonPressMode()
{
	currentAnim_ = (currentAnim_+1) % (int)Anim::Count;
	start_animation_if_new(animations_[currentAnim_]);
}