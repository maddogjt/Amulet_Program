#pragma once

#include <cstdint>
#include <Arduino.h>
#include "globals.h"
#include <FastLED.h>
FASTLED_USING_NAMESPACE

struct animColor
{
	uint8_t h;
	uint8_t s;
	uint8_t v;
};

struct animParams
{
	animColor color1_;
	animColor color2_;
	animColor color3_;
	uint8_t speed_;
	uint8_t flags_;
	uint8_t extra_[6];
};

// These are used by 'raster' style animations
typedef struct
{
	float_t x;
	float_t y;
} position_t;

inline float_t distance(float_t x1, float_t x2, float_t y1, float_t y2)
{
	return sqrtf((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

// TODO: remove?
extern uint8_t gHue;
extern CRGB gLeds[RGB_LED_COUNT];

#define ANIMATION_FLAG_FOLD (0x01)			   /**< Top 4 leds mirror bottom 4 leds */
#define ANIMATION_FLAG_MIRROR (0x02)		   /**< The left side mirrors the right side */
#define ANIMATION_FLAG_LOOP (0x04)			   /**< The left side mirrors the right side, but upside down */
#define ANIMATION_FLAG_USE_SIGNAL_POWER (0x08) /**< Set signal power parameter */
#define ANIMATION_FLAG_NO_GRADIENT (0x10)	  /**< Just use the defined colors */
#define ANIMATION_FLAG_FULL_SPECTRUM (0x20)	/**< Use full spectrum instead of a gradient */

void mirror();

class Animation
{
public:
	virtual ~Animation() {}

	// temporary, switch to using params_
	int animation_p1;
	int animation_p2;

public:
	// Animation(const animParams &parameters) : params_(parameters) {}
	void setParams(const animParams &parameters)
	{
		params_ = parameters;
		animation_p1 = params_.extra_[0];
		animation_p2 = params_.extra_[1];
	}
	virtual void init() {}
	virtual void step(const int frame, const float deltaTime, const float sourceDistance) = 0;

	animParams params_;
};
