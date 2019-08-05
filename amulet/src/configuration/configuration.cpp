#include "configuration.h"
#include "../communication/uart.h"
#include "../animation/animation_modifiers.h"

#define ARDUINO_GENERIC
#include <FastLED.h>
#undef ARDUINO_GENERIC

#include "../../led.h"
#include "../../signal.h"
#include "../../settings.h"
#include "../../Startup.h"
#include "../../AnimationMod.h"

static uint8_t g_AnimIdx = 0;
static uint8_t g_ConfigIdx = 0;
static bool g_tweaker_is_modifying_animation = true;

static StartupConfig config = defaultConfigForRemoteSetup();
static animPattern ambient = config.pattern;

void colorCycle(bool next, uint8_t idx)
{
	uint8_t hue = (idx == 1) ? ambient.params.color1_ : ambient.params.color2_;
	hue = (hue + 255 + (next ? 16 : -16)) % 255;
	uart_stream().printf("P: color%d V: %d\n", idx, hue);
	if (idx == 1)
	{
		ambient.params.color1_ = hue;
	}
	else
	{
		ambient.params.color2_ = hue;
	}
}

void extraCycle(bool next, uint8_t idx)
{
	uint8_t &extra = (idx == 7) ? ambient.params.extra0_ : ambient.params.extra1_;
	extra = (extra + 255 + (next ? 16 : -16)) % 255;
	uart_stream().printf("P: extra[%d] V: %d\n", (idx == 7) ? 0 : 1, extra);
}

void speedCycle(bool next, uint8_t unused)
{
	uint8_t speed = ambient.params.speed_;
	speed = (speed + 255 + (next ? 16 : -16)) % 255;
	uart_stream().printf("P: speed V: %d\n", speed);
	ambient.params.speed_ = speed;
}

void flagCycle(bool next, uint8_t idx)
{
	const int effectCount = (int)AnimationModifier::Count;

	uint8_t modifier = 
		(ambient.params.flags_ + effectCount + (next ? 1 : -1)) % effectCount;
	uart_stream().printf("P: flag V: %s\n", animation_modifier_get_name((AnimationModifier)modifier));
	ambient.params.flags_ = modifier;
}

void maskCycle(bool next, uint8_t unused)
{
	uint8_t mask = ambient.params.mask_;
	mask = (mask + 256 + (next ? 1 : -1)) % 256;
	char buf[32];
	getMaskName(buf, mask);
	uart_stream().printf("P: mask V: %s\n", buf);
	ambient.params.mask_ = mask;
}

void filterCycle(bool next, uint8_t unused)
{
	uint8_t filter = ambient.params.filter_;
	filter = (filter + 256 + (next ? 1 : -1)) % 256;
	char buf[32];
	getFilterName(buf, filter);
	uart_stream().printf("P: filter V: %s\n", buf);
	ambient.params.filter_ = filter;
}

void animCycle(bool next, uint8_t unused)
{
	uint8_t anim_idx = (uint8_t)ambient.name;
	anim_idx = (anim_idx + get_animations_count() + (next ? 1 : -1)) % get_animations_count();
	uart_stream().printf("P:anim V:%.10s\n", get_animation_name((Anim)anim_idx));
	ambient.name = (Anim)anim_idx;
}

void modeCycle(bool next, uint8_t unused)
{
	uint8_t mode_idx = 0;
	const amulet_mode_t modes[] = {AMULET_MODE_AMULET, AMULET_MODE_BEACON, AMULET_MODE_RUNE, AMULET_MODE_BEACON_POWER_AMULET};
	for (int i = 0; i < 4; i++)
	{
		if (modes[i] == config.mode)
		{
			mode_idx = i;
			break;
		}
	}

	mode_idx = (mode_idx + 4 + (next ? 1 : -1)) % 4;
	uart_stream().printf("P: mode V: %s\n", get_config_mode_name(modes[mode_idx]));
	config.mode = modes[mode_idx];
	if (config.mode == AMULET_MODE_BEACON)
	{
		config.ad.power = 80;
		config.ad.decay = 128;
		config.ad.range = -80;
	}
	if (config.mode == AMULET_MODE_RUNE)
	{
		config.ad.power = 150;
		config.ad.decay = 96;
		config.ad.range = -90;
	}
	if (config.mode == AMULET_MODE_BEACON_POWER_AMULET)
	{
		config.ad.power = 180;
		config.ad.decay = 160;
		config.ad.range = -70;
	}
	if (config.mode == AMULET_MODE_AMULET)
	{
		config.ad.power = 80;
		config.ad.decay = 64;
		config.ad.range = -70;
	}
}

void powerCycle(bool next, uint8_t unused)
{
	uint8_t power = config.ad.power;
	power = (power + 255 + (next ? 16 : -16)) % 255;
	uart_stream().printf("P: power V: %d\n", power);
	config.ad.power = power;
}

void decayCycle(bool next, uint8_t unused)
{
	uint8_t decay = config.ad.decay;
	decay = (decay + 255 + (next ? 16 : -16)) % 255;
	uart_stream().printf("P: decay V: %4.2f%%\n", (float)decay * 100.f / 255.f);
	config.ad.decay = decay;
}

void rangeCycle(bool next, uint8_t unused)
{
	int8_t range = config.ad.range;
	range = max(-120, min(0, range + (next ? -2 : 2)));
	uart_stream().printf("P: range V: %d\n", range);
	config.ad.range = range;
}

void animRSSICycle(bool next, uint8_t unused)
{
	ambient.params.flags_ ^= ANIMATION_FLAG_USE_SIGNAL_POWER;
	uart_stream().printf("P: anim rssi V: %d\n", ambient.params.flags_ & ANIMATION_FLAG_USE_SIGNAL_POWER);
}

typedef void (*ParameterCycleList[])(bool next, uint8_t idx);
ParameterCycleList g_ConfigCyclers = {
	modeCycle,
	powerCycle,
	decayCycle,
	rangeCycle,
	animRSSICycle,
};
const char *g_ConfigCyclerNames[] = {
	"mode",
	"power",
	"decay",
	"range",
	"AnimRSSI",
};

ParameterCycleList g_AnimCyclers = {
	animCycle,
	colorCycle,
	colorCycle,
	speedCycle,
	flagCycle,
	maskCycle,
	filterCycle,
	extraCycle,
	extraCycle,
};
const char *g_AnimCyclerNames[] = {
	"animation",
	"color 1",
	"color 2",
	"speed",
	"flag",
	"mask",
	"filter",
	"extra 1",
	"extra 2",
};



void configuration_handle_command(const char *str, size_t len)
{

	if (str[0] != '!')
	{
		Serial.println("uart_stream() service got rogue packet");
		Serial.printBuffer(str, len);
		return;
	}

	// Color
	if (str[1] == 'C')
	{
		uint8_t red = str[2];
		uint8_t green = str[3];
		uint8_t blue = str[4];
		Serial.print("RGB #");
		if (red < 0x10)
			Serial.print("0");
		Serial.print(red, HEX);
		if (green < 0x10)
			Serial.print("0");
		Serial.print(green, HEX);
		if (blue < 0x10)
			Serial.print("0");
		Serial.println(blue, HEX);

		CHSV color = rgb2hsv_approximate(CRGB(red, green, blue));

		ambient.params.color1_ = color.hue;

		// Set the ambient animation
		led_set_ambient_animation(ambient);
		return;
	}

	// Button
	if (str[1] == 'B')
	{
		uint8_t button = str[2];
		uint8_t press = str[3];
		// uint8_t unknown = str[4];
		if (press != '0')
		{
			// Only do things on button release
			return;
		}
		if (button == '5') // up
		{
			if (g_tweaker_is_modifying_animation)
			{
				g_AnimIdx = (g_AnimIdx + 8) % 9;
				uart_stream().printf("P: %s\n", g_AnimCyclerNames[g_AnimIdx]);
			}
			else
			{
				g_ConfigIdx = (g_ConfigIdx + 4) % 5;
				uart_stream().printf("P: %s\n", g_ConfigCyclerNames[g_ConfigIdx]);
			}
		}
		else if (button == '6') // down
		{
			if (g_tweaker_is_modifying_animation)
			{
				g_AnimIdx = (g_AnimIdx + 1) % 9;
				uart_stream().printf("P: %s\n", g_AnimCyclerNames[g_AnimIdx]);
			}
			else
			{
				g_ConfigIdx = (g_ConfigIdx + 1) % 5;
				uart_stream().printf("P: %s\n", g_ConfigCyclerNames[g_ConfigIdx]);
			}
		}
		else if (button == '7' || button == '8') // left or right
		{
			if (g_tweaker_is_modifying_animation)
			{
				g_AnimCyclers[g_AnimIdx](button == '8', g_AnimIdx);
			}
			else
			{
				g_ConfigCyclers[g_ConfigIdx](button == '8', g_ConfigIdx);
			}
		}
		else if (button == '1') // The Info Button
		{
			if (g_tweaker_is_modifying_animation)
			{
				uart_stream().printf("Anim: %s\n", get_animation_name(ambient.name));
			}
			else
			{
				uart_stream().printf("Conf: %s\n", get_config_mode_name(config.mode));
			}
		}
		else if (button == '2') // The Info Details Button
		{
			// Turn off the harsh blue light for animation configuration
			digitalWrite(LED_BUILTIN, !LED_STATE_ON);
			if (g_tweaker_is_modifying_animation)
			{
				uart_stream().printf("mask: %d filter: %d\n", ambient.params.mask_, ambient.params.filter_);
			}
			else
			{
				uart_stream().printf(":P\n");
			}
		}
		else if (button == '3') // The mode switcher button
		{
			if (g_tweaker_is_modifying_animation)
			{
				uart_stream().printf("Editing config\n");
				g_tweaker_is_modifying_animation = false;
			}
			else
			{
				uart_stream().printf("Editing anim\n");
				g_tweaker_is_modifying_animation = true;
			}
		}
		else if (button == '4') // Commit the changes button (after system reset)
		{
			uart_stream().printf("Saving Config\n");
			config.pattern = ambient;

			// char buf[120];
			// serializeAnimPattern(buf, 120, config.pattern); // Serializing just for the Serial log
			localSettings_.startupConfig_ = config;
			if (config.mode == AMULET_MODE_BEACON_POWER_AMULET)
			{
				// Save this separate so its never overwritten (such as ambient patterns)
				localSettings_.powerPattern_ = config.pattern;
			}

			write_local_settings();

			delay(500);
			uart_stream().printf("Restarting\n");
			NVIC_SystemReset();
		}

		// Set the ambient animation
		led_set_ambient_animation(ambient);
		return;
	}

	Serial.print("[Prph] Unknown BLE Uart command RX: ");
	Serial.println(str);
}