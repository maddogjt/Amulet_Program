#pragma once
#include "mode.h"
#include <Arduino.h>
#include "../animation/animation_config.h"
#include "../animation/animation_list.h"
#include "../communication/advertising.h"

class Signal;

class LampMode : public AmuletMode
{ 
public:
	void start() override;
	void loop() override;
	void buttonActionReset(button_action_t action) override;
	void buttonActionMode(button_action_t action) override;

	bool modePressed_ = false;
	bool resetPressed_ = false;

	void restorePreviousMode();
	static amulet_mode_t previousMode_;
};

void start_lamp_mode(amulet_mode_t returnMode);