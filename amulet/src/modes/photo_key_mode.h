#pragma once
#include "mode.h"
#include <Arduino.h>
#include "../animation/animation_config.h"
#include "../animation/animation_list.h"

class Signal;

class PhotoKeyMode : public AmuletMode
{
public:
	void start() override;
	void loop() override;
	void buttonPressMode() override;
	void startPuzzle();
	void startVictory();
};