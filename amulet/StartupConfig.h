#pragma once

#include "animations.h"
#include "src/communication/advertising.h"
#include "src/communication/packet_types.h"

typedef enum
{
	AMULET_MODE_FIRSTBOOT,
	AMULET_MODE_CONFIG,
	AMULET_MODE_AMULET,
	AMULET_MODE_BEACON,
	AMULET_MODE_RUNE,
	AMULET_MODE_BEACON_POWER_AMULET,
	AMULET_MODE_COUNT,
} amulet_mode_t;

const char *get_config_mode_name(amulet_mode_t mode);

struct StartupConfig
{
	// Top level mode
	amulet_mode_t mode;

	advertisementParams ad;

	// The animation
	anim_config_t pattern;

	bool operator==(const StartupConfig &c) const
	{
		return mode == c.mode && ad == c.ad && pattern == c.pattern;
	}
};

StartupConfig defaultConfigForRemoteSetup();