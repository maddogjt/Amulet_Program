#pragma once

#include "animations.h"
#include "src/communication/advertising.h"
#include "src/communication/packet_types.h"
#include "src/modes/mode.h"


struct StartupConfig
{
	// Top level mode
	amulet_mode_t mode;
	bool enterConfigMode_;

	advertisementParams ad;

	// The animation
	anim_config_t burnPattern_{};

	// bool operator==(const StartupConfig &c) const
	// {
	// 	return mode == c.mode && ad == c.ad && pattern == c.pattern;
	// }

	anim_config_t &getConfigModeAnim(amulet_mode_t mode) {
		if (mode == AMULET_MODE_BURN)
		{
			return burnPattern_;
		} else {
			return burnPattern_;
		}
	}
};