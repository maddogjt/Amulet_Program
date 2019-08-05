#pragma once

#include <FastLED.h>
#include "Animation.h"
#include "signal.h"

FASTLED_USING_NAMESPACE

enum class Anim
{
	Unknown = -1,
#define DEFINE_ANIM(name) name,
#include "AnimList.hpp"
#undef DEFINE_ANIM
	Count,
};

struct animPattern
{
	Anim name;
	animParams params;
	bool operator==(const animPattern &p) const
	{
		return name == p.name && params == p.params;
	}
};

void dump_animation_to_console(const animPattern &anim);

void start_animation(const animPattern &pattern);
bool matches_current_animation(const animPattern &pattern);

void step_animation(Signal *topSignal);

const char *get_animation_name(Anim anim);
int get_animations_count();

// Functions that should be adapted to animations or removed
void fold();
void mirror_invert();
void confetti();
void sinelon();
void mirror();
void loop_leds();

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
