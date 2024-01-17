#include "blinky_mode.h"
#include "lamp_mode.h"
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
		altAnimations_[i] = {};
		switch (i)
		{
		case 0:
			animations_[i].anim_ = Anim::AnimPrism;
			animations_[i].modifiers_ = AnimationModifier::Ocean;
			altAnimations_[i].anim_ = Anim::AnimPrism;
			break;
		case 1:
			animations_[i].anim_ = Anim::AnimRainbow;
			animations_[i].speed_ = 4;
			altAnimations_[i].anim_ = Anim::AnimRainbow;
			altAnimations_[i].modifiers_ = AnimationModifier::Blush;
			altAnimations_[i].speed_ = 4;
			break;
		case 2:
			animations_[i].anim_ = Anim::AnimTwister;
			altAnimations_[i].anim_ = Anim::AnimTwister;
			altAnimations_[i].modifiers_ = AnimationModifier::Shift;
			break;
		case 3:
			animations_[i].anim_ = Anim::AnimBurn;
			altAnimations_[i].anim_ = Anim::AnimBurn;
			altAnimations_[i].modifiers_ = AnimationModifier::Ocean;
			break;
		case 4:
			animations_[i].anim_ = Anim::AnimJump;
			altAnimations_[i].anim_ = Anim::AnimJump;
			altAnimations_[i].modifiers_ = AnimationModifier::FoldAutumn;
			break;
		case 5:
			animations_[i].anim_ = Anim::AnimOrbit;
			altAnimations_[i].anim_ = Anim::AnimOrbit;
			altAnimations_[i].modifiers_ = AnimationModifier::Bifold;
			break;
		case 6:
			animations_[i].anim_ = Anim::AnimTwinkle;
			altAnimations_[i].anim_ = Anim::AnimTwinkle;
			altAnimations_[i].extra0_ = 1;
			break;
		case 7:
			animations_[i].anim_ = Anim::AnimInNOut;
			altAnimations_[i].anim_ = Anim::AnimInNOut;
			altAnimations_[i].modifiers_ = AnimationModifier::Scramble;
			altAnimations_[i].filter_ = OverlayFilter::Right;
			break;
		case 8:
			animations_[i].anim_ = Anim::AnimCylon;
			altAnimations_[i].anim_ = Anim::AnimCylon;
			altAnimations_[i].modifiers_ = AnimationModifier::Shift;
			break;
		case 9:
			animations_[i].anim_ = Anim::AnimJuggle;
			animations_[i].modifiers_ = AnimationModifier::Ocean;
			altAnimations_[i].anim_ = Anim::AnimJuggle;
			altAnimations_[i].modifiers_ = AnimationModifier::Autumn;
			break;
		case 10:
			animations_[i].anim_ = Anim::AnimPsyco;
			animations_[i].modifiers_ = AnimationModifier::Ocean;
			altAnimations_[i].anim_ = Anim::AnimPsyco;
			altAnimations_[i].modifiers_ = AnimationModifier::Mirror;
			altAnimations_[i].filter_ = OverlayFilter::Darken;
			altAnimations_[i].color1_ = CRGB::Teal;
			break;
		case 11:
			animations_[i].anim_ = Anim::AnimShadow;
			altAnimations_[i].anim_ = Anim::AnimShadow;
			altAnimations_[i].modifiers_ = AnimationModifier::Shift;
			break;
		case 12:
			animations_[i].anim_ = Anim::AnimBPM;
			animations_[i].modifiers_ = AnimationModifier::Blush;
			animations_[i].speed_ = 32;
			altAnimations_[i].anim_ = Anim::AnimBPM;
			altAnimations_[i].modifiers_ = AnimationModifier::Mirror;
			altAnimations_[i].speed_ = 16;
			break;
		case 13:
			animations_[i].anim_ = Anim::AnimConfetti;
			altAnimations_[i].anim_ = Anim::AnimConfetti;
			altAnimations_[i].modifiers_ = AnimationModifier::Shift;
			break;
		default:
			animations_[i].anim_ = (Anim)i;
			altAnimations_[i].anim_ = (Anim)i;
			altAnimations_[i].modifiers_ = AnimationModifier::Shift;
			break;
		}
	}

	displayCodeAnim_ = {};
	displayCodeAnim_.anim_ = Anim::AnimDisplayInt;

	mimicRestingAnim_ = {};
	mimicRestingAnim_.anim_ = Anim::AnimBounce;
	mimicRestingAnim_.color2_ = CRGB::CornflowerBlue;

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
		if (millis() < lastSignalTime_ + 10000) {
			// Keep a seen pattern for at least 10s before losing it
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
	advertising_stop();
	if (currentAnim_ < 14) {
		auto anim = animAltMode_ ? altAnimations_[currentAnim_] : animations_[currentAnim_];
		advertising_start(
			adType,
			localSettings_.startupConfig_.ad,
			(uint8_t *)&anim,
			sizeof(anim));
	} else if (currentAnim_ == 15) {
		advertising_start(
			adType,
			localSettings_.startupConfig_.ad,
			(uint8_t *)&localSettings_.startupConfig_.burnPattern_,
			sizeof(localSettings_.startupConfig_.burnPattern_));
	}
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
			if (s->data_.signal_type == (uint8_t)AdvertisementType::Runic) {
				if (newPattern == lastSignalPattern_) {
					if (lastRuneTime_ + 30000 < millis() && !(localSettings_.startupConfig_.burnPattern_ == newPattern)) {
						// We've been near this rune for a while, save it to config
						localSettings_.startupConfig_.burnPattern_ = newPattern;
						write_local_settings();
					}
				} else {
					// its a different pattern than the last signal.
					// Reset the rune time
					lastRuneTime_ = millis();
				}
			} else {
				lastRuneTime_ = 0;
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
		digitalWrite(LED_BUILTIN, !LED_STATE_ON);
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
		updateAmuletAdvertising = true;
		this->start_current_anim();
	}
}

void BlinkyMode::broadcastAction() {
	updateAmuletAdvertising = false;
	start_animation_if_new(animations_[currentAnim_]);

	sendingPattern_ = true;
	sendPatternStop_ = millis() + 15000;

	this->advertiseCurrentPattern(AdvertisementType::Beacon);
	digitalWrite(LED_BUILTIN, LED_STATE_ON);
}

void BlinkyMode::nextAnimationAction() {
	currentAnim_ = (currentAnim_+1) % 16;
	displayCodeAnim_.extra0_ = currentAnim_ + (animAltMode_ ? 16 : 0);

	start_animation_if_new(displayCodeAnim_);
	changeAnimTime_ = millis();
	localSettings_.blinkyAnimIndex_ = currentAnim_;
}

void BlinkyMode::altAnimationAction() {
	animAltMode_ = !animAltMode_;
	displayCodeAnim_.extra0_ = currentAnim_ + (animAltMode_ ? 16 : 0);

	start_animation_if_new(displayCodeAnim_);
	changeAnimTime_ = millis();
	localSettings_.blinkyAltAnim_ = animAltMode_;
}

void BlinkyMode::ledBrightnessAction() {
	led_next_brightness();
	brightnessChanged_ = true;
}

// Brightness/Power Action
void BlinkyMode::buttonActionReset(button_action_t action)
{
	if (ignoreNextButton)
	{
		if (action != BUTTON_ACTION_PRESS) {
			ignoreNextButton = false;
		}
		return;
	}

	if (action == BUTTON_ACTION_PRESS)
	{
		resetPressed_ = true;
		if (modePressed_)
		{
			start_lamp_mode(AMULET_MODE_BLINKY);
		}
	} else if (action == BUTTON_ACTION_RELEASE) {
		resetPressed_ = false;
		this->ledBrightnessAction();
	
	} else if (action == BUTTON_ACTION_LONG_PRESS_RELEASE) {
		resetPressed_ = false;
		this->broadcastAction();
	}
}

// Change Animation
void BlinkyMode::buttonActionMode(button_action_t action) {

	if (ignoreNextButton)
	{
		if (action != BUTTON_ACTION_PRESS) {
			ignoreNextButton = false;	
		}
		return;
	}

	if (action == BUTTON_ACTION_PRESS)
	{
		modePressed_ = true;
		if (resetPressed_)
		{
			start_lamp_mode(AMULET_MODE_BLINKY);
		}
	}
	else if (action == BUTTON_ACTION_RELEASE)
	{
		modePressed_ = false;
		this->nextAnimationAction();
	}
	else if (action == BUTTON_ACTION_LONG_PRESS_START)
	{
		modePressed_ = false;
		this->altAnimationAction();
	}
}