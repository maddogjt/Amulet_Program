#pragma once
#include "mode.h"

class ConfigMode : public AmuletMode {
	public:
	void start() override;
	void loop() override;
};