#pragma once

#include "src/animation/animation_config.h"

#include <FastLED.h>
#include "Animation.h"
#include "signal.h"

FASTLED_USING_NAMESPACE


void dump_animation_to_console(const anim_config_t &anim);

void start_animation(const anim_config_t &pattern);
bool matches_current_animation(const anim_config_t &pattern);

void start_animation_if_new(const anim_config_t &pattern);

void step_animation();

const char *animation_get_name(Anim anim);

// helper func for rssi based anims
inline float normalizeRSSI(int8_t rssi)
{
	// max strength is RSSI >= -64. min strength at -128
	float normalizedRSSI = min(64.f, max(0.f, (float)rssi + 128.f));
	normalizedRSSI /= 64.f;
	return normalizedRSSI;

	// Tested and works! Examples:
	// Test RSSI: 2 => 1.00
	// Test RSSI: -70 => 0.91
	// Test RSSI: -99 => 0.45
	// Test RSSI: -128 => 0.00
}
