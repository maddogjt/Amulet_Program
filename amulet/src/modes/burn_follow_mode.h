#pragma once
#include "mode.h"
#include <Arduino.h>
#include "../animation/animation_config.h"
#include "../animation/animation_list.h"

class Signal;

class BurnFollowMode : public AmuletMode
{ 
public:
	void start() override;
	void loop() override;

	anim_config_t blackout_;

	void set_animation_from_signal(Signal *s);

private: 
	void maybeChangePattern(const anim_config_t &anim);
};