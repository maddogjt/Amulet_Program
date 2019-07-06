#pragma once

#define ARDUINO_GENERIC
#include <FastLED.h>
#undef ARDUINO_GENERIC
#include "Animation.h"

FASTLED_USING_NAMESPACE

extern CRGB gLeds[RGB_LED_COUNT];

enum class Anim
{
	Unknown,
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

animPattern deserializeAnimPattern(char *str, uint8_t len);
uint8_t serializeAnimPattern(char *buffer, const uint8_t len, const animPattern &pattern);
void start_animation(const animPattern &pattern);
void start_animation(Anim name, int p1, int p2);
bool matches_current_animation(Anim name, int p1, int p2);
bool matches_current_animation(const animPattern &pattern);

void step_animation();

// Functions that should be adapted to animations or removed
void fold();
void mirror_invert();
void confetti();
void sinelon();
void mirror();
