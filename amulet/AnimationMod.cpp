#include "AnimationMod.h"
#include "animations/RasterHelper.h"

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

void modLED(CRGB &led, const CRGB &overlay, const float_t alpha, const uint8_t mod)
{
	if (mod == 0)
	{
		// no mod
		return;
	}
	else if (mod == 1)
	{
		// Shift the hue around the RGB wheel
		rotateColorLeft(led, alpha);
	}
	else if (mod == 2)
	{
		// Shift the hue around the RGB wheel
		rotateColorRight(led, alpha);
	}
	else if (mod == 3)
	{
		// Shift the hue around the RGB wheel
		rotateColorAcross(led, alpha);
	}
	else if (mod == 4)
	{
		// Darken by alpha
		led.nscale8(alpha * 255);
	}
	else if (mod == 5)
	{
		// blend with input
		led = blend(led, overlay, alpha * 255);
	}
	else if (mod == 6)
	{
		// difference blend
		led = led - overlay * (uint8_t)(alpha * 255);
	}
	else if (mod == 7)
	{
		// additive blend
		led = led + overlay * (uint8_t)(alpha * 255);
	}
}

float_t alphas[RGB_LED_COUNT];

void setRasterAngleAlphas(float_t angle, bool wave)
{
	float x;
	float y;
	for (int i = 0; i < RGB_LED_COUNT; i++)
	{
		x = led_positions()[i].x + 5; // need to offset by 5 to avoid 0s mucking up math
		y = led_positions()[i].y + 5;
		float alpha = atanf(y / x);
		// Serial.printf("alpha: %f  (y:%f / x:%f)\n", alpha, y, x);
		float c = distance(0, x, 0, y);
		// Serial.printf("c: %f     angle: %f\n", c, angle);

		float hue = c * cosf(alpha - angle);
		alphas[i] = !wave ? (hue / 255) : ((1 + cosf(hue * 2 * PI / 255)) / 2);
	}
}

void sinelonAlphas()
{
	// a colored dot sweeping back and forth, with fading trails
	for (int i = 0; i < RGB_LED_COUNT; i++)
	{
		alphas[i] = alphas[i] - 0.08;
	}
	int pos = beatsin16(30, 0, RGB_LED_COUNT - 1);
	alphas[pos] += 0.75;
}

void modLEDs(CRGB *leds, const uint8_t len, const uint8_t mod)
{
	static bool need_to_clear_alphas = true;
	if (need_to_clear_alphas)
	{
		memset(alphas, 0.f, sizeof(alphas));
		need_to_clear_alphas = false;
	}

	static float_t angle = 0;

	uint8_t overlayMod = mod & 0xF0;
	overlayMod = overlayMod >> 4;
	uint8_t colorMod = mod & 0x0F;
	// Serial.printf("Mod %d > overlay %d and color %d\n");

	if (overlayMod == 0)
	{
		sinelonAlphas();
	}
	else if (overlayMod == 1)
	{
		setRasterAngleAlphas(angle, false);
		angle += 0.05;
	}
	else if (overlayMod == 2)
	{
		setRasterAngleAlphas(angle, true);
		angle += 0.05;
	}

	// Clamp alphas and apply mods
	for (int i = 0; i < RGB_LED_COUNT; i++)
	{
		alphas[i] = max(0, min(1.0, alphas[i]));
		CRGB overlay = CRGB(0, 0, 0);
		modLED(leds[i], overlay, alphas[i], colorMod);
	}
}