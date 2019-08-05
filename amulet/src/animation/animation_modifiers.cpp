#include "animation_modifiers.h"
#include <FastLED.h>

static void blur_leds(CRGB leds[], size_t count);
static void fold(CRGB leds[], size_t count);
static void scramble_leds(CRGB leds[], size_t count);
static void flip_leds(CRGB leds[], size_t count);
static void loop_leds(CRGB leds[], size_t count);
static void cycle_leds(int msPerCycle, CRGB leds[], size_t count);
static void mirror(CRGB leds[], size_t count);
static void mirror_invert(CRGB leds[], size_t count);

constexpr char* kAnimationModifierNames[] {
	"None",
	"Fold",
	"Mirror",
	"Loop",
	"Cycle",
	"Shift",
	"Blur",
	"Flip",
	"Scramble"
};

const char *animation_modifier_get_name(AnimationModifier modifier)
{
	return kAnimationModifierNames[(int)modifier];
}


void animation_modifier_apply(AnimationModifier modifier, CRGB leds[], size_t count)
{
	switch (modifier)
	{
	case AnimationModifier::Fold:
		fold(leds, count);
		break;
	case AnimationModifier::Mirror:
		mirror(leds, count);
		break;
	case AnimationModifier::Loop:
		loop_leds(leds, count);
		break;
	case AnimationModifier::Cycle:
		cycle_leds(500, leds, count);
		break;
	case AnimationModifier::Shift:
		mirror_invert(leds, count);
		break;
	case AnimationModifier::Blur:
		blur_leds(leds, count);
		break;
	case AnimationModifier::Flip:
		flip_leds(leds, count);
		break;
	case AnimationModifier::Scramble:
		scramble_leds(leds, count);
		break;
	default:
		break;
	}
}

void fold(CRGB leds[], size_t count)
{
	if (count < 8)
	{
		return;
	}

	/* *
	+------+
	\ 4  3 /
	| 5  2 |
	| 6  1 |
	/ 7  0 \
	+------+
	*/
	leds[2] = leds[1];
	leds[3] = leds[0];
	leds[4] = leds[7];
	leds[5] = leds[6];
}

void swap(CRGB &c1, CRGB &c2)
{
	CRGB swap = c1;
	c1 = c2;
	c2 = swap;
}

void blur_leds(CRGB leds[], size_t count)
{
	if (count != 8)
	{
		return;
	}

	CRGB rows[8];
	rows[0] = leds[7];
	rows[1] = leds[0];
	rows[2] = leds[6];
	rows[3] = leds[1];
	rows[4] = leds[5];
	rows[5] = leds[2];
	rows[6] = leds[4];
	rows[7] = leds[3];
	blurRows(rows, 4, 2, 40);
	leds[7] = rows[0];
	leds[0] = rows[1];
	leds[6] = rows[2];
	leds[1] = rows[3];
	leds[5] = rows[4];
	leds[2] = rows[5];
	leds[4] = rows[6];
	leds[3] = rows[7];
}

void scramble_leds(CRGB leds[], size_t count)
{
	if (count != 8)
	{
		return;
	}
	CRGB scramble[8];
	scramble[0] = leds[3];
	scramble[1] = leds[1];
	scramble[2] = leds[0];
	scramble[3] = leds[6];
	scramble[4] = leds[5];
	scramble[5] = leds[4];
	scramble[6] = leds[7];
	scramble[7] = leds[2];
	for (int i = 0; i < 8; i++)
	{
		leds[i] = scramble[i];
	}
}

void flip_leds(CRGB leds[], size_t count)
{
	if (count != 8)
	{
		return;
	}

	swap(leds[7], leds[4]);
	swap(leds[6], leds[5]);
}

void loop_leds(CRGB leds[], size_t count)
{
	// bottom right => top left, top right => bottom left
	for (int i = 0; i < count / 2; i++)
	{
		leds[i + count / 2] = leds[i];
	}
}

int cycleStart = 0;
void cycle_leds(int msPerCycle, CRGB leds[], size_t count)
{
	if (count > 64)
	{
		return;
	}
	EVERY_N_MILLISECONDS(msPerCycle)
	{
		cycleStart = (cycleStart + 1) % count;
	}
	// Need a copy of leds so we don't overwrite before cycle;
	CRGB temp[64];
	for (int i = 0; i < count; i++)
	{
		temp[i] = leds[i];
	}

	for (int i = 0; i < count; i++)
	{
		leds[i] = temp[(i + cycleStart) % count];
	}
}

void mirror_invert(CRGB leds[], size_t count)
{
	for (int i = 0; i < count / 2; i++)
	{
		leds[count - 1 - i] = CRGB(
			leds[i].g,
			leds[i].b,
			leds[i].r);
	}
}

void mirror(CRGB leds[], size_t count)
{
	for (int i = 0; i < count / 2; i++)
	{
		leds[count - 1 - i] = leds[i];
	}
}
