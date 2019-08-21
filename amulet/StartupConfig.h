#pragma once

#include "animations.h"
#include "src/communication/advertising.h"
#include "src/communication/packet_types.h"

typedef enum
{
	AMULET_MODE_FIRSTBOOT,
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
	bool enterConfigMode_;

	advertisementParams ad;

	// The animation
	anim_config_t ambientPattern_{};
	anim_config_t beaconPattern_{};
	anim_config_t powerPattern_{};
	anim_config_t runePattern_{};

	// bool operator==(const StartupConfig &c) const
	// {
	// 	return mode == c.mode && ad == c.ad && pattern == c.pattern;
	// }

	anim_config_t &getConfigModeAnim(amulet_mode_t mode) {
		if (mode == AMULET_MODE_AMULET) {
			return ambientPattern_;
		}
		else if (mode == AMULET_MODE_BEACON)
		{
			return beaconPattern_;
		}
		else if (mode == AMULET_MODE_RUNE)
		{
			return runePattern_;
		}
		else if (mode == AMULET_MODE_BEACON_POWER_AMULET)
		{
			return powerPattern_;
		} else {
			return ambientPattern_;
		}
	}
};