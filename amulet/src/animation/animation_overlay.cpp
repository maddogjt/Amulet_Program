#include "animation_overlay.h"
#include "animations.h"

#define DO_INCLUDES
#include "animation_list.hpp"

constexpr const char *kFilterNames[]{
	"None",
	"Left",
	"Right",
	"Across",
	"Darken",
	"Lighten",
	"MaskOnTop",
	"MaskUnder",
	"Blend"
};

template <typename T>
T safe(const T mode) {
	return (T)((int)mode % (int)T::Count);
}

static Animation *gCurrentOverlay = nullptr;
static OverlayFilter gCurrentFilter = OverlayFilter::None;

const char *animation_overlay_get_filter_name(OverlayFilter filter_number)
{
	return kFilterNames[(int)safe(filter_number)];
}

void animation_overlay_set(Anim animation, OverlayFilter filter)
{
	gCurrentFilter = filter;
	if (gCurrentOverlay != nullptr)
	{
		delete gCurrentOverlay;
		gCurrentOverlay = nullptr;
	}

	switch (safe(animation))
	{
#define DEFINE_ANIM(animName)             \
	case Anim::animName:                  \
		gCurrentOverlay = new animName(); \
		break;

#include "animation_list.hpp"
#undef DEFINE_ANIM

	default:
		LOG_LV1("ANIM", "Bad animation name");
		break;
	}

	if (gCurrentOverlay)
	{
		gCurrentOverlay->init();
	}
}

void modLED(CRGB &led, const CRGB &overlay, const uint8_t alpha, const OverlayFilter mod);

void maskAndFilter(CRGB *leds, const uint8_t len, int frame, bool use_custom_alpha, int alpha)
{
	if (gCurrentOverlay == nullptr)
	{
		return;
	}
	gCurrentOverlay->step(frame, 0, 1.f);

	for (int i = 0; i < RGB_LED_COUNT; i++)
	{
		CHSV color = rgb2hsv_approximate(gCurrentOverlay->leds[i]);
		modLED(leds[i], gCurrentOverlay->leds[i], use_custom_alpha ? alpha : color.value, gCurrentFilter);
	}
}

void animation_overlay_apply(CRGB *leds, const uint8_t len, int frame)
{
	maskAndFilter(leds, len, frame, false, 0);
}
void animation_overlay_apply(CRGB *leds, const uint8_t len, int frame, int alpha)
{
	maskAndFilter(leds, len, frame, true, alpha);
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

void modLED(CRGB &led, const CRGB &overlay, const uint8_t alpha, const OverlayFilter mod)
{
	float_t fAlpha = alpha;
	fAlpha /= 255.f;

	if (mod == OverlayFilter::None)
	{
		// no mod
		return;
	}
	else if (mod == OverlayFilter::Left)
	{
		// Shift the hue around the RGB wheel
		rotateColorLeft(led, fAlpha);
	}
	else if (mod == OverlayFilter::Right)
	{
		// Shift the hue around the RGB wheel
		rotateColorRight(led, fAlpha);
	}
	else if (mod == OverlayFilter::Across)
	{
		// Shift the hue around the RGB wheel
		rotateColorAcross(led, fAlpha);
	}
	else if (mod == OverlayFilter::Darken)
	{
		// DARKEN
		led &= overlay;
	}
	else if (mod == OverlayFilter::Lighten)
	{
		// LIGHTEN
		led |= overlay;
	}
	else if (mod == OverlayFilter::MaskOnTop)
	{
		// MASK ON TOP
		led = blend(led, overlay, alpha);
		// for (int i = 0; i < RGB_LED_COUNT; i++) {
		// 	CHSV color = rgb2hsv_approximate(gCurrentOverlay->leds[i]);
		// 	led[i] = (1 / 1 + exp())
			
		// }
	}
	else if (mod == OverlayFilter::MaskUnder)
	{
		// MASK UNDER
		CHSV ledcolor = rgb2hsv_approximate(led);
		led = blend(overlay, led, ledcolor.value);
	}
	else if (mod == OverlayFilter::Blend)
	{
		// EVEN BLEND
		led = blend(led, overlay, 128);
	}
}
