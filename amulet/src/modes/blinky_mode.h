#pragma once
#include "mode.h"
#include <Arduino.h>
#include "../animation/animation_config.h"
#include "../animation/animation_list.h"
#include "../communication/advertising.h"

class Signal;

class BlinkyMode : public AmuletMode
{ 
public:
	void start() override;
	void loop() override;
	void buttonPressReset() override;
	void buttonHoldReset() override;
	void buttonPressMode() override;
	void buttonHoldMode() override;
	
	// startup
	bool ignoreNextButton = true;

	// LED brightness
	bool brightnessChanged_ = true;

	// Beacon broadcasting
	uint32_t sendPatternStop_ = 0;
	bool sendingPattern_ = false;

	// Changing the amulet mode
	uint32_t changeAnimTime_ = 0;
	anim_config_t displayCodeAnim_;
	bool updateAmuletAdvertising = false;

	// There are 14 'basic' animations, the 15th is mimic, the 16th is runic
	anim_config_t animations_[14];
	anim_config_t altAnimations_[14];
	int currentAnim_ = 0;
	bool animAltMode_ = false;
	anim_config_t currentAnimConfig_;

	// Mimic pattern info
	anim_config_t mimicRestingAnim_;
	anim_config_t altMimicRestingAnim_;
	anim_config_t lastSignalPattern_;
	uint32_t lastSignalTime_ = 0;
	uint32_t lastRuneTime_ = 0;

	void advertiseCurrentPattern(const AdvertisementType adType=AdvertisementType::Amulet);

	void broadcastAction();
	void ledBrightnessAction();
	void nextAnimationAction();
	void altAnimationAction();
	void start_current_anim();
	void set_animation_from_signal(Signal *s);
};