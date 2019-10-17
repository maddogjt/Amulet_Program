#pragma once
#include "mode.h"
#include <Arduino.h>
#include "../animation/animation_config.h"
#include "../animation/animation_list.h"

class Signal;

class SeacompAmuletMode : public AmuletMode
{ 
public:
	void start() override;
	void loop() override;
	void buttonHoldReset() override;
	void buttonPressMode() override;

	uint32_t sendPatternStop_ = 0;
	bool sendingPattern_ = false;

	anim_config_t animations_[(int)Anim::Count];
	int currentAnim_ = 0;

	void set_animation_from_signal(Signal *s);
};