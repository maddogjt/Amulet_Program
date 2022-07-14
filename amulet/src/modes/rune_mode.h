#pragma once
#include "mode.h"
#include <Arduino.h>
#include "../animation/animation_config.h"
#include "../animation/animation_list.h"
#include "../communication/advertising.h"

class Signal;

class RuneMode : public AmuletMode
{ 
public:
	void start() override;
	void loop() override;
	void buttonPressReset(bool released) override;
	void buttonPressMode(bool released) override;
	
	// startup
	bool ignoreNextButton = true;

	// LED brightness
	bool brightnessChanged_ = true;

	void ledBrightnessAction();
};