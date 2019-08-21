#include "configuration.h"
#include "../communication/uart.h"
#include "../animation/animation_modifiers.h"
#include "../animation/animation_overlay.h"
#include "../../BrightnessMode.h"

#include <FastLED.h>

#include "../../led.h"
#include "../../signal.h"
#include "../../settings.h"
#include "../../Startup.h"

static uint8_t g_AnimIdx = 0;
static uint8_t g_ConfigIdx = 0;
static bool g_tweaker_is_modifying_animation = true;

StartupConfig& getConfig() {
	return localSettings_.startupConfig_;
}

anim_config_t& getAnim() {
	return getConfig().getConfigModeAnim(getConfig().mode);
}

void colorCycle(bool next, uint8_t idx)
{
	uint8_t hue = (idx == 1) ? getAnim().color1_ : getAnim().color2_;
	hue = (hue + 255 + (next ? 16 : -16)) % 255;
	uart_stream().printf("P: color%d V: %d\n", idx, hue);
	if (idx == 1)
	{
		getAnim().color1_ = hue;
	}
	else
	{
		getAnim().color2_ = hue;
	}
}

void extraCycle(bool next, uint8_t idx)
{
	uint8_t &extra = (idx == 7) ? getAnim().extra0_ : getAnim().extra1_;
	extra = (extra + 255 + (next ? 16 : -16)) % 255;
	uart_stream().printf("P: extra[%d] V: %d\n", (idx == 7) ? 0 : 1, extra);
}

void speedCycle(bool next, uint8_t unused)
{
	uint8_t speed = getAnim().speed_;
	speed = (speed + 255 + (next ? 16 : -16)) % 255;
	uart_stream().printf("P: speed V: %d\n", speed);
	getAnim().speed_ = speed;
}


int32_t cycle(bool next, int32_t value, int32_t count)
{
	if (next) {
		return (value + 1) % count;
	} else {
		return value > 0 ? (value-1) : (count-1);
	}
}

template <typename T>
T cycleEnum(bool next, T value)
{
	return (T)cycle(next, (int32_t)value, (int32_t)T::Count);
}

void flagCycle(bool next, uint8_t idx)
{
	getAnim().modifiers_ = cycleEnum(next, getAnim().modifiers_);
	uart_stream().printf("P: flag V: %s\n", animation_modifier_get_name(getAnim().modifiers_));
}

void maskCycle(bool next, uint8_t unused)
{
	getAnim().overlay_ = cycleEnum(next, getAnim().overlay_);
	uart_stream().printf("P: mask V: %s\n", animation_get_name(getAnim().overlay_));
}

void filterCycle(bool next, uint8_t unused)
{
	getAnim().filter_ = cycleEnum(next, getAnim().filter_);
	uart_stream().printf("P: filter V: %s\n", 
		animation_overlay_get_filter_name(getAnim().filter_));
}

void animCycle(bool next, uint8_t unused)
{
	getAnim().anim_ = cycleEnum(next, getAnim().anim_);
	uart_stream().printf("P:anim V:%.10s\n", 
		animation_get_name(getAnim().anim_));
}

void modeCycle(bool next, uint8_t unused)
{
	uint8_t mode_idx = 0;
	const amulet_mode_t modes[] = {AMULET_MODE_AMULET, AMULET_MODE_BEACON, AMULET_MODE_RUNE, AMULET_MODE_BEACON_POWER_AMULET};
	for (int i = 0; i < 4; i++)
	{
		if (modes[i] == getConfig().mode)
		{
			mode_idx = i;
			break;
		}
	}

	mode_idx = (mode_idx + 4 + (next ? 1 : -1)) % 4;
	uart_stream().printf("P: mode V: %s\n", get_config_mode_name(modes[mode_idx]));
	auto &config = getConfig();
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
	uint8_t power = getConfig().ad.power;
	power = (power + 255 + (next ? 16 : -16)) % 255;
	uart_stream().printf("P: power V: %d\n", power);
	getConfig().ad.power = power;
}

void decayCycle(bool next, uint8_t unused)
{
	uint8_t decay = getConfig().ad.decay;
	decay = (decay + 255 + (next ? 16 : -16)) % 255;
	uart_stream().printf("P: decay V: %4.2f%%\n", (float)decay * 100.f / 255.f);
	getConfig().ad.decay = decay;
}

void rangeCycle(bool next, uint8_t unused)
{
	int8_t range = getConfig().ad.range;
	range = max(-120, min(0, range + (next ? -2 : 2)));
	uart_stream().printf("P: range V: %d\n", range);
	getConfig().ad.range = range;
}

void animRSSICycle(bool next, uint8_t unused)
{
	// getAnim().flags_ ^= ANIMATION_FLAG_USE_SIGNAL_POWER;
	// uart_stream().printf("P: anim rssi V: %d\n", getAnim().flags_ & ANIMATION_FLAG_USE_SIGNAL_POWER);
}

void bikeModeCycle(bool next, uint8_t unused)
{
	localSettings_.bikeMode_ = !localSettings_.bikeMode_;
	// getAnim().flags_ ^= ANIMATION_FLAG_USE_SIGNAL_POWER;
	// uart_stream().printf("P: anim rssi V: %d\n", getAnim().flags_ & ANIMATION_FLAG_USE_SIGNAL_POWER);
}

void brightCycle(int index, bool next) {
	int increment = (next ? 4 : -4);
	localSettings_.brightness_[index] += increment;
	// safety check
	localSettings_.brightness_[index] = localSettings_.brightness_[index] % 128;
	uart_stream().printf("P: b%d V: %d\n", index, localSettings_.brightness_[index]);
	refreshBrightness();
}

void brightness0Cycle(bool next, uint8_t unused) {
	brightCycle(0, next);
}

void brightness1Cycle(bool next, uint8_t unused)
{
	brightCycle(1, next);
}

void brightness2Cycle(bool next, uint8_t unused)
{
	brightCycle(2, next);
}

typedef void (*ParameterCycleList[])(bool next, uint8_t idx);
ParameterCycleList g_ConfigCyclers = {
	modeCycle,
	powerCycle,
	decayCycle,
	rangeCycle,
	animRSSICycle,
	bikeModeCycle,
	brightness0Cycle,
	brightness1Cycle,
	brightness2Cycle,
};
const char *g_ConfigCyclerNames[] = {
	"mode",
	"power",
	"decay",
	"range",
	"AnimRSSI",
	"bikeMode",
	"bright0",
	"bright1",
	"bright2",
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
	"speed",
	"color 1",
	"color 2",
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

		getAnim().color1_ = color.hue;

		// Set the ambient animation
		led_set_ambient_animation(getAnim());
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
				uart_stream().printf("Anim: %s\n", animation_get_name(getAnim().anim_));
			}
			else
			{
				uart_stream().printf("Conf: %s\n", get_config_mode_name(getConfig().mode));
			}
		}
		else if (button == '2') // The Info Details Button
		{
			// Turn off the harsh blue light for animation configuration
			digitalWrite(LED_BUILTIN, !LED_STATE_ON);
			if (g_tweaker_is_modifying_animation)
			{
				uart_stream().printf("mask: %d filter: %d\n", getAnim().overlay_, getAnim().filter_);
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
			//config.pattern = ambient;

			// char buf[120];
			// serializeAnimPattern(buf, 120, config.pattern); // Serializing just for the Serial log
			// localSettings_.startupConfig_ = config;
			// if (config.mode == AMULET_MODE_BEACON_POWER_AMULET)
			// {
			// 	// Save this separate so its never overwritten (such as ambient patterns)
			// 	localSettings_.startuppowerPattern_ = config.pattern;
			// }

			write_local_settings();

			delay(500);
			uart_stream().printf("Restarting\n");
			NVIC_SystemReset();
		}

		// Set the ambient animation
		auto &anim = localSettings_.startupConfig_.getConfigModeAnim(localSettings_.startupConfig_.mode);
		led_set_ambient_animation(anim);
		return;
	}

	Serial.print("[Prph] Unknown BLE Uart command RX: ");
	Serial.println(str);
}