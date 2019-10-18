#pragma once
#include "mode.h"
#include <Arduino.h>
#include "../animation/animation_config.h"
#include "../animation/animation_list.h"
#include "../misc/simon_sequence.h"

class Signal;

class SimonMode : public AmuletMode
{ 
	enum class State
	{
		Playing,
		Failure,
	};
public:
	void start() override;
	void loop() override;
	void buttonPressMode() override;
private:
	void startNewSequence(int level);
	void startStep(int stepIndex);
	void startRoundVictory(SimonColor color);
	void startFinalVictory();
	void handlePressedState(int state);

	void startFail();
	void endFail();

	uint8_t simonSeed_;

	SimonStep simonSteps_[kMaxSimonSteps];
	size_t simonStepCount_ = 0;
	uint currentSimonStep_ = 0;
	int stepEnd_ = 0;

	int currentLevel_ = 0;
	State state_ {State::Playing};
};