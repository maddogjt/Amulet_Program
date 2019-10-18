#include "seacomp_amulet_mode.h"
#include "../animation/animations.h"
#include "../communication/advertising.h"
#include "../communication/signal.h"
#include "../settings/settings.h"
#include "../leds/led.h"

void SeacompAmuletMode::start()
{
	Serial.println("Starting seacomp mode");
	anim_ = {};
	anim_.anim_ = Anim::AnimScoreboard;
	anim_.speed_ = 4;
	// Set the initial ambient animation
	auto &config = localSettings_.startupConfig_;
	start_animation(anim_);
	advertising_start(AdvertisementType::Amulet, config.ad,
					  (uint8_t *)&anim_, sizeof(anim_config_t));
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
		start_animation_if_new(anim_);
	}
}

void SeacompAmuletMode::loop()
{
	EVERY_N_MILLISECONDS(globalSettings_.animationUpdateTimer_)
	{
		Signal *signal = signal_get_current_top();
		set_animation_from_signal(signal);
	}
}