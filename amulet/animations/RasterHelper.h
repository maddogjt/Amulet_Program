#pragma once

#include "../Animation.h"

// These are used by 'raster' style animations
// Raster style animations use a mapping of the LEDs into a 2d plane.

typedef struct
{
	float_t x;
	float_t y;
} position_t;

inline float_t distance(float_t x1, float_t x2, float_t y1, float_t y2)
{
	return sqrtf((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

position_t led_positions[] =
	{
		{.x = 122, .y = 0},
		{.x = 108, .y = 52},
		{.x = 108, .y = 104},
		{.x = 122, .y = 156},
		{.x = 0, .y = 156},
		{.x = 14, .y = 104},
		{.x = 14, .y = 52},
		{.x = 0, .y = 0},
};

position_t led_positions2[] =
	{
		{.x = 61, .y = -78},
		{.x = 47, .y = -26},
		{.x = 47, .y = 26},
		{.x = 61, .y = 78},
		{.x = -61, .y = 78},
		{.x = -47, .y = 26},
		{.x = -47, .y = -26},
		{.x = -61, .y = -78},
};
