#pragma once

#include <cstdint>
#include "animation_list.h"

struct CRGB;

enum class OverlayFilter : uint8_t
{
	None,
	Left,
	Right,
	Across,
	Darken,
	Lighten,
	MaskOnTop, 
	MaskUnder,
	Blend,

	Count
};

const char *animation_overlay_get_filter_name(OverlayFilter filter);

void animation_overlay_set(Anim animation, OverlayFilter filter);
void animation_overlay_apply(CRGB *leds, const uint8_t len, int frame);
void animation_overlay_apply(CRGB *leds, const uint8_t len, int frame, int alpha);
