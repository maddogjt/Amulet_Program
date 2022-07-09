#include "blinky_mode.h"
#include "../animation/animations.h"
#include "../communication/signal.h"
#include "../settings/settings.h"
#include "../leds/led.h"

#define MIMIC_IDX 14
#define RUNIC_IDX 15

void BlinkyMode::start() {
	LOG_LV1("MODE", "Blinky start");

	// When waking from power off, the button used to wake should not trigger an action.
	ignoreNextButton = true;

	for (int i = 0; i < 14; i++) {
		animations_[i] = {};
		animations_[i].anim_ = (Anim)(i);
		animations_[i].speed_ = 4;

		altAnimations_[i] = {};
		altAnimations_[i].anim_ = (Anim)(i);
		switch (i)
		{
		case (int)Anim::AnimSolidHue:
		case (int)Anim::AnimRainbow:
		case (int)Anim::AnimTwister:
		case (int)Anim::AnimBurn:
		case (int)Anim::AnimJump:
		case (int)Anim::AnimOrbit:
		case (int)Anim::AnimTwinkle:
		case (int)Anim::AnimInNOut:
		case (int)Anim::AnimCylon:
		case (int)Anim::AnimJuggle:
		case (int)Anim::AnimPsyco:
		case (int)Anim::AnimSinelon:
		case (int)Anim::AnimBPM:
		case (int)Anim::AnimConfetti:
			altAnimations_[i].modifiers_ = AnimationModifier::Shift;
	
		default:
			break;
		}
	}

	displayCodeAnim_ = {};
	displayCodeAnim_.anim_ = Anim::AnimDisplayInt;

	mimicRestingAnim_ = {};
	mimicRestingAnim_.anim_ = Anim::AnimBounce;

	// Set the initial animation
	currentAnim_ = localSettings_.blinkyAnimIndex_;
	animAltMode_ = localSettings_.blinkyAltAnim_;

	auto &config = localSettings_.startupConfig_;
	start_animation(animations_[currentAnim_]);
	advertising_start(AdvertisementType::Amulet, config.ad, 
		(uint8_t *)&animations_[currentAnim_], sizeof(anim_config_t));
}

void BlinkyMode::start_current_anim()
{
	if (millis() < changeAnimTime_ + 1000) {
		// We are still displaying the anim int.
		return;
	}

	if (currentAnim_ < 14)
	{
		set_mimic_mode(false);
		if (animAltMode_) {
			start_animation_if_new(altAnimations_[currentAnim_]);
		} else {
			start_animation_if_new(animations_[currentAnim_]);
		}
	}
	else if (currentAnim_ == MIMIC_IDX) {
		// Mimic mode
		set_mimic_mode(true);
		if (millis() < lastSignalTime_ + 5000) {
			start_animation_if_new(lastSignalPattern_);
		} else {
			start_animation_if_new(mimicRestingAnim_);
		}
	}
	else if (currentAnim_ == RUNIC_IDX) {
		// Pattern from last Rune
		set_mimic_mode(false);
		start_animation_if_new(localSettings_.startupConfig_.burnPattern_);
	}
}

void BlinkyMode::advertiseCurrentPattern(const AdvertisementType adType)
{
	// Start Amulet broadcast
	// BUG: will crash if currentAnim is too high
	auto anim = animAltMode_ ? altAnimations_[currentAnim_] : animations_[currentAnim_];
	advertising_stop();
	advertising_start(
		adType,
		localSettings_.startupConfig_.ad,
		(uint8_t *)&anim,
		sizeof(anim));
}

// returns true if animation was changed
void BlinkyMode::set_animation_from_signal(Signal *s)
{
	bool usingSignalPattern = true;
	if (s != nullptr)
	{
		if (s->data_.signal_type == (uint8_t)AdvertisementType::Amulet && currentAnim_ != MIMIC_IDX) {
			// This happens immediately after switching away from mimic mode because we don't
			// clear out the amulet signals.
			usingSignalPattern = false;
		} else {
			// Set Runic signals here and save them
			anim_config_t newPattern;
			VERIFY_STATIC(sizeof(newPattern) <= kMaxPayloadLen);
			memcpy(&newPattern, s->data_.payload, sizeof(anim_config_t));
			
			lastSignalTime_ = millis();
			if (newPattern == lastSignalPattern_) {
				if (lastRuneTime_ + 30000 < millis() && !(localSettings_.startupConfig_.burnPattern_ == newPattern)) {
					// We've been near this rune for a while, save it to config
					localSettings_.startupConfig_.burnPattern_ = newPattern;
					write_local_settings();
				}
			} else {
				// its a differnt pattern than the last signal.
				// Reset the rune time
				lastRuneTime_ = (s->data_.signal_type == (uint8_t)AdvertisementType::Runic) ? millis() : 0;
			}
			lastSignalPattern_ = newPattern;
			start_animation_if_new(lastSignalPattern_);
		}
	}
	else if (millis() > changeAnimTime_ + 1000)
	{
		this->start_current_anim();
	}
}

void BlinkyMode::loop() {

	// Stop Broadcasting if it's time
	if (sendingPattern_ && millis() > sendPatternStop_)
	{
		//digitalWrite(LED_BUILTIN, !LED_STATE_ON);
		sendingPattern_ = false;

		this->advertiseCurrentPattern();
		this->start_current_anim();
	}

	// Check for incoming broadcast,
	// And adjust advertistment
	EVERY_N_MILLISECONDS(globalSettings_.animationUpdateTimer_)
	{
		// Adjust Advertisement
		if (brightnessChanged_) {
			brightnessChanged_ = false;
			if (led_get_brightness() == LedBrightness::Off ) {
				advertising_stop();
			} else {
				this->advertiseCurrentPattern();
			}
		} else if( updateAmuletAdvertising ) {
			updateAmuletAdvertising = false;
			this->advertiseCurrentPattern();
		}

		// if we aren't broadcasting, check the local signals.
		// But also don't do this if we are displaying the anim change pattern.
		if (!sendingPattern_ && changeAnimTime_==0)
		{
			Signal *signal = signal_get_current_top();
			set_animation_from_signal(signal);
		}
	}

	// after showing the code start the anim
	if ( changeAnimTime_ != 0 && millis() > changeAnimTime_ + 1000)
	{
		changeAnimTime_ = 0;
		this->start_current_anim();
	}
}

void BlinkyMode::broadcastAction() {
	updateAmuletAdvertising = false;
	start_animation_if_new(animations_[currentAnim_]);

	sendingPattern_ = true;
	sendPatternStop_ = millis() + 15000;

	this->advertiseCurrentPattern(AdvertisementType::Beacon);
}

void BlinkyMode::nextAnimationAction() {
	currentAnim_ = (currentAnim_+1) % 16;
	displayCodeAnim_.extra0_ = currentAnim_;
	updateAmuletAdvertising = true;

	start_animation_if_new(displayCodeAnim_);
	changeAnimTime_ = millis();

	localSettings_.blinkyAnimIndex_ = currentAnim_;
}

void BlinkyMode::altAnimationAction() {
	animAltMode_ = !animAltMode_;
	localSettings_.blinkyAltAnim_ = animAltMode_;

	updateAmuletAdvertising = true;
	changeAnimTime_ = millis();
	this->start_current_anim();
}

void BlinkyMode::ledBrightnessAction() {
	led_next_brightness();
	brightnessChanged_ = true;
}

// Brightness/Power Action
void BlinkyMode::buttonPressReset() {
	LOG_LV1("MODE", "Blinky press reset");
	if (!ignoreNextButton) {
		this->ledBrightnessAction();
	} else {
		ignoreNextButton = false;
	}
}

// Broadcast
void BlinkyMode::buttonHoldReset() {
	LOG_LV1("MODE", "Blinky hold reset");
	if (!ignoreNextButton) {
		this->broadcastAction();
	} else {
		ignoreNextButton = false;
	}
}

// Change Animation
void BlinkyMode::buttonPressMode() {
	LOG_LV1("MODE", "Blinky press mode");
	if (!ignoreNextButton) {
		this->nextAnimationAction();
	} else {
		ignoreNextButton = false;
	}
}

// Alt Animation
void BlinkyMode::buttonHoldMode() {
	LOG_LV1("MODE", "Blinky hold mode");
	if (!ignoreNextButton) {
		this->altAnimationAction();
	} else {
		ignoreNextButton = false;
	}
}