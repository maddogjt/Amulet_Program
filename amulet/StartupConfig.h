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
	amulet_mode_t mode;
	animPattern pattern;

	// signal type
	uint8_t power;
	uint8_t decay;
	int8_t range;
};

void deserializeAnimPacket(animPattern &pattern, uint8_t *buffer, uint8_t len);
void deserializeModeAndSignal(StartupConfig &config, uint8_t *buffer, uint8_t len);
