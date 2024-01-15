#include "burn_mode.h"
#include "../animation/animations.h"
#include "../communication/advertising.h"
#include "../communication/signal.h"
#include "../settings/settings.h"
#include "../leds/led.h"

void BurnMode::start()
{
	// advertising_stop();

	for (int i = 0; i < (int)Anim::Count; i++)
	{
		animations_[i] = {};
		animations_[i].anim_ = (Anim)(i);
		animations_[i].speed_ = 16;
	}
	// Set the initial ambient animation
	// auto &config = localSettings_.startupConfig_;

	maybeChangePattern(animations_[currentAnim_]);
	// start_animation(animations_[currentAnim_]);
	// advertising_start(AdvertisementType::Amulet, config.ad,
	// (uint8_t *)&animations_[currentAnim_], sizeof(anim_config_t));
}

// returns true if animation was changed
void BurnMode::set_animation_from_signal(Signal *s)
{
	if (s != nullptr)
	{
		anim_config_t pattern;
		VERIFY_STATIC(sizeof(pattern) <= kMaxPayloadLen);
		memcpy(&pattern, s->data_.payload, sizeof(anim_config_t));
		maybeChangePattern(pattern);
	}
	else
	{
		
		maybeChangePattern(animations_[currentAnim_]);
	}
}

// char (*__kaboom)[sizeof(anim_config_t)] = 1;

void BurnMode::maybeChangePattern(const anim_config_t &anim)
{
	if (start_animation_if_new(anim))
	{
		advertising_stop();

		advertising_start(
			AdvertisementType::Amulet,
			localSettings_.startupConfig_.ad,
			(uint8_t *)&anim,
			sizeof(anim_config_t));
	}
}

void BurnMode::loop()
{
	if (sendingPattern_ && millis() > sendPatternStop_)
	{
		//digitalWrite(LED_BUILTIN, !LED_STATE_ON);
		sendingPattern_ = false;
		maybeChangePattern(animations_[currentAnim_]);
	}

	EVERY_N_MILLISECONDS(globalSettings_.animationUpdateTimer_)
	{
		// led_override_brightness(false);
		if (!sendingPattern_)
		{
			Signal *signal = signal_get_current_top();
			set_animation_from_signal(signal);
			// if (localSettings_.startupConfig_.externalLedEn_ && signal)
			// {
			// led_override_brightness(true);
			// }
		}
	}
}

void BurnMode::buttonHoldReset()
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

void BurnMode::buttonPressMode()
{
	currentAnim_ = (currentAnim_ + 1) % (int)Anim::Count;
	maybeChangePattern(animations_[currentAnim_]);
}