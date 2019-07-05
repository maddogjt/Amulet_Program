#pragma once
#include "animations.h"

typedef enum
{
	AMULET_MODE_STARTUP,
	AMULET_MODE_AMULET,
	AMULET_MODE_BEACON,
	AMULET_MODE_RUNE,
	AMULET_MODE_POWER_AMULET,
	AMULET_MODE_COUNT,
} amulet_mode_t;

struct StartupConfig
{
	// Top level mode
	amulet_mode_t mode;

	// signal type
	uint8_t power;
	uint8_t decay;
	int8_t range;

	// The animation
	animPattern pattern;

	bool operator==(const StartupConfig &c) const
	{
		return mode == c.mode && power == c.power && decay == c.decay && range == c.range && pattern == c.pattern;
	}
};

void testSerialization();
StartupConfig deserializeStartupConfig(char *str, uint8_t len);
