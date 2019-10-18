#pragma once
#include "mode.h"
#include <Arduino.h>
#include "../animation/animation_config.h"
#include "../animation/animation_list.h"
#include "../misc/simon_sequence.h"

class Signal;

class SimonMode : public AmuletMode
{ 
public:
	void start() override;
	void loop() override;
	void buttonPressMode() override;
private:
	void startNewSequence();
	void startStep(int stepIndex);

	uint8_t simonSeed_;
	uint8_t simonLevel_;

	SimonStep simonSteps_[kMaxSimonSteps];
	size_t simonStepCount_ = 0;
	size_t currentSimonStep_ = 0;
	int stepEnd_ = 0;
};