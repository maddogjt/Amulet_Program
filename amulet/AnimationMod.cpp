#include "AnimationMod.h"
#define DO_INCLUDES
#include "AnimList.hpp"

#include "animations/RasterHelper.h"
#include "animations.h"

Anim animForMask(const int mask_number)
{
	return (Anim)((mask_number) % get_animations_count());
}

int getMaskName(char *buf, const uint8_t mask_number)
{
	sprintf(buf, "%s", get_animation_name(animForMask(mask_number)));
	return strlen(buf);
}

Animation *maskAnimation = nullptr;
void initMask(const uint8_t mask_number)
{
	if (maskAnimation != nullptr)
	{
		delete maskAnimation;
		maskAnimation = nullptr;
	}

	switch (animForMask(mask_number))
	{
#define DEFINE_ANIM(animName)           \
	case Anim::animName:                \
		maskAnimation = new animName(); \
		break;

#include "AnimList.hpp"
#undef DEFINE_ANIM

	default:
		LOG_LV1("ANIM", "Bad animation name");
		break;
	}

	if (maskAnimation)
	{
		maskAnimation->init();
	}
}

const int FILTER_COUNT = 9;
int getFilterName(char *buf, const uint8_t filter_number)
{
	const char *filterNames[] = {"None", "Left", "Right", "Across", "Darken", "Lighten", "MaskOnTop", "MaskUnder", "blend"};
	return sprintf(buf, "%s", filterNames[filter_number % FILTER_COUNT]);
}

void modLED(CRGB &led, const CRGB &overlay, const uint8_t alpha, const uint8_t mod);

void maskAndFilter(CRGB *leds, const uint8_t len, const uint8_t filter_number, int frame, bool use_custom_alpha, int alpha)
{
	if (maskAnimation == nullptr)
	{
		return;
	}
	maskAnimation->step(frame, 0, 1.f);

	for (int i = 0; i < RGB_LED_COUNT; i++)
	{
		CHSV color = rgb2hsv_approximate(maskAnimation->leds[i]);
		modLED(leds[i], maskAnimation->leds[i], use_custom_alpha ? alpha : color.value, filter_number % FILTER_COUNT);
	}
}
void maskAndFilter(CRGB *leds, const uint8_t len, const uint8_t filter_number, int frame)
{
	maskAndFilter(leds, len, filter_number, frame, false, 0);
}
void maskAndFilter(CRGB *leds, const uint8_t len, const uint8_t filter_number, int frame, int alpha)
{
	maskAndFilter(leds, len, filter_number, frame, true, alpha);
}

void rotateColorLeft(CRGB &led, const float_t alpha)
{
	uint8_t r = led.r;
	uint8_t g = led.g;
	uint8_t b = led.b;
	float_t same = 1.f - alpha;
	led.r = r * same + g * alpha;
	led.g = g * same + b * alpha;
	led.b = b * same + r * alpha;
}

void rotateColorRight(CRGB &led, const float_t alpha)
{
	uint8_t r = led.r;
	uint8_t g = led.g;
	uint8_t b = led.b;
	float_t same = 1.f - alpha;
	led.r = r * same + b * alpha;
	led.g = g * same + r * alpha;
	led.b = b * same + g * alpha;
}

void rotateColorAcross(CRGB &led, const float_t alpha)
{
	uint8_t r = led.r;
	uint8_t g = led.g;
	uint8_t b = led.b;
	float_t same = 1.f - alpha;
	led.r = r * same + (g / 2 + b / 2) * alpha;
	led.g = g * same + (r / 2 + b / 2) * alpha;
	led.b = b * same + (r / 2 + g / 2) * alpha;
}

void modLED(CRGB &led, const CRGB &overlay, const uint8_t alpha, const uint8_t mod)
{
	float_t fAlpha = alpha;
	fAlpha /= 255.f;

	if (mod == 0)
	{
		// no mod
		return;
	}
	else if (mod == 1)
	{
		// Shift the hue around the RGB wheel
		rotateColorLeft(led, fAlpha);
	}
	else if (mod == 2)
	{
		// Shift the hue around the RGB wheel
		rotateColorRight(led, fAlpha);
	}
	else if (mod == 3)
	{
		// Shift the hue around the RGB wheel
		rotateColorAcross(led, fAlpha);
	}
	else if (mod == 4)
	{
		// DARKEN
		led &= overlay;
	}
	else if (mod == 5)
	{
		// LIGHTEN
		led |= overlay;
	}
	else if (mod == 6)
	{
		// MASK ON TOP
		led = blend(led, overlay, alpha);
	}
	else if (mod == 7)
	{
		// MASK UNDER
		CHSV ledcolor = rgb2hsv_approximate(led);
		led = blend(overlay, led, ledcolor.value);
	}
	else if (mod == 8)
	{
		// EVEN BLEND
		led = blend(led, overlay, 128);
	}
}
