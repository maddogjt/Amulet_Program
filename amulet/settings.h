#pragma once

#include <Arduino.h>
#include "Adafruit_LittleFS.h"
#include "StartupConfig.h"

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
} LocalSettings;

void write_global_settings();
void write_local_settings();
void settings_init();

extern GlobalSettings globalSettings_;
extern LocalSettings localSettings_;