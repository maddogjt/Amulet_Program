#pragma once

#include <Arduino.h>

#include "../animation/animation_config.h"
#include "../communication/advertising.h"
#include "../communication/packet_types.h"
#include "../modes/mode.h"

struct StartupConfig
{
	// Top level mode
	amulet_mode_t mode;
	bool enterConfigMode_;

	advertisementParams ad;

	// The animation
	anim_config_t burnPattern_{};
	anim_config_t simonPattern_{};
	anim_config_t seacompAmuletPattern_{};

	// bool operator==(const StartupConfig &c) const
	// {
	// 	return mode == c.mode && ad == c.ad && pattern == c.pattern;
	// }

	anim_config_t &getConfigModeAnim(amulet_mode_t mode)
	{
		if (mode == AMULET_MODE_BURN)
		{
			return burnPattern_;
		} else if (mode == AMULET_MODE_SIMON)
		{
			return simonPattern_;
		}
		else if (mode == AMULET_MODE_SEACOMP_AMULET)
		{
			return seacompAmuletPattern_;
		}
		else
		{
			return burnPattern_;
		}
	}
};

typedef struct __attribute__((packed))
{
	int16_t signature_;
	uint8_t version_;
	uint8_t brightnessOld_[3];
	int8_t txPower_;
	int8_t runeSeenCountThreshold_; // how long do we have to hang around a rune for its pattern to stick
	int8_t ambientPowerThreshold_;  // If top signal is lower power than this, show ambient
	int16_t animationUpdateTimer_;  // in milliseconds. This determines how often we check a new top signal and change anims.

	uint8_t pad_[8];
} GlobalSettings;

static_assert(sizeof(GlobalSettings) == 19, "");

typedef struct __attribute__((packed))
{
	int16_t signature_;
	uint8_t version_;
	size_t configSize_;
	StartupConfig startupConfig_;
	uint8_t brightness_[3];

	bool bikeMode_;
	bool bikeExtend_;
} LocalSettings;

void write_global_settings();
void write_local_settings();
void settings_init();

extern GlobalSettings globalSettings_;
extern LocalSettings localSettings_;