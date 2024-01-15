#include "burn_follow_mode.h"
#include "../animation/animations.h"
#include "../communication/advertising.h"
#include "../communication/signal.h"
#include "../settings/settings.h"
#include "../leds/led.h"

void BurnFollowMode::start()
{
	// advertising_stop();

	blackout_ = {};
	blackout_.anim_ = Anim::AnimBlackout;
	blackout_.speed_ = 16;

	maybeChangePattern(blackout_);
}

int32_t noSignalTime_ = 0;
int32_t last_ = millis();

// returns true if animation was changed
void BurnFollowMode::set_animation_from_signal(Signal *s)
{
	auto now = millis();
	if (s != nullptr)
	{
		anim_config_t pattern;
		VERIFY_STATIC(sizeof(pattern) <= kMaxPayloadLen);
		memcpy(&pattern, s->data_.payload, sizeof(anim_config_t));
		// start_animation_if_new(pattern);
		maybeChangePattern(pattern);
		noSignalTime_ = 0;
	}
	else
	{
		noSignalTime_ += now - last_;
		if (noSignalTime_ > 10000)
		{

			maybeChangePattern(blackout_);
		}
	}
	last_ = now;
}

// char (*__kaboom)[sizeof(anim_config_t)] = 1;

void BurnFollowMode::maybeChangePattern(const anim_config_t &anim)
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

void BurnFollowMode::loop()
{
	EVERY_N_MILLISECONDS(globalSettings_.animationUpdateTimer_)
	{
		Signal *signal = signal_get_current_top();
		set_animation_from_signal(signal);
	}
}