#pragma once

#include <FastLED.h>
#include "Animation.h"

FASTLED_USING_NAMESPACE

extern CRGB gLeds[RGB_LED_COUNT];
extern int animation_p1;
extern int animation_p2;

enum class Anim
{
	Unknown,
#define DEFINE_ANIM(name) name,
#include "AnimList.hpp"
#undef DEFINE_ANIM
};

void start_animation( Anim name, int p1, int p2);
bool matches_current_animation(Anim name, int p1, int p2);

void step_animation( );

// Functions that should be adapted to animations or removed
void rainbow2();
void confetti();
void sinelon();
void mirror();

