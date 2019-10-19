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
	//	void buttonHoldReset() override;
	void buttonPressMode() override;

	void set_animation_from_signal(Signal *s);
	anim_config_t scoreboardAnim_{};
	anim_config_t victoryAnim_{};

	uint8_t gameState_;
	int timeAtTree_;
};