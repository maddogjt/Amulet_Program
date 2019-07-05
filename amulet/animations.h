#pragma once

#include <FastLED.h>
#include "Animation.h"

FASTLED_USING_NAMESPACE

extern CRGB gLeds[RGB_LED_COUNT];

enum class Anim
{
	Unknown,
#define DEFINE_ANIM(name) name,
#include "AnimList.hpp"
#undef DEFINE_ANIM
};

struct animPattern
{
	Anim name;
	animParams params;
};

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
