#pragma once
#include "animations.h"
#include "ble.h"

typedef enum
{
	AMULET_MODE_CONFIG,
	AMULET_MODE_AMULET,
	AMULET_MODE_BEACON,
	AMULET_MODE_RUNE,
	AMULET_MODE_BEACON_POWER_AMULET,
	AMULET_MODE_RUNIC_POWER_AMULET,
	AMULET_MODE_COUNT,
} amulet_mode_t;

struct StartupConfig
{
	// Top level mode
	amulet_mode_t mode;

	advertisementParams ad;

	// The animation
	animPattern pattern;

	bool operator==(const StartupConfig &c) const
	{
		return mode == c.mode && ad == c.ad && pattern == c.pattern;
	}
};

void testSerialization();
StartupConfig deserializeStartupConfig(char *str, uint8_t len);
