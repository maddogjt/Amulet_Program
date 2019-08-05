#include "configuration.h"
#include "../communication/uart.h"

#include <bluefruit.h>
#define ARDUINO_GENERIC
#include <FastLED.h>
#undef ARDUINO_GENERIC

#include "../../led.h"
#include "../../signal.h"
#include "../../settings.h"
#include "../../Startup.h"
#include "../../AnimationMod.h"

static StartupConfig config = defaultConfigForRemoteSetup();
static animPattern ambient = config.pattern;

void colorCycle(bool next, uint8_t idx)
{
	uint8_t hue = (idx == 1) ? ambient.params.color1_ : ambient.params.color2_;
	hue = (hue + 255 + (next ? 16 : -16)) % 255;
	uart_get_service().printf("P: color%d V: %d\n", idx, hue);
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
	uart_get_service().printf("P: extra[%d] V: %d\n", (idx == 7) ? 0 : 1, extra);
}

void speedCycle(bool next, uint8_t unused)
{
	uint8_t speed = ambient.params.speed_;
	speed = (speed + 255 + (next ? 16 : -16)) % 255;
	uart_get_service().printf("P: speed V: %d\n", speed);
	ambient.params.speed_ = speed;
}

void flagCycle(bool next, uint8_t idx)
{
	uint8_t flags_idx = 0;
	uint8_t flags[] = {0,
					   ANIMATION_EFFECT_FOLD,
					   ANIMATION_EFFECT_MIRROR,
					   ANIMATION_EFFECT_LOOP,
					   ANIMATION_EFFECT_CYCLE,
					   ANIMATION_EFFECT_SHIFT,
					   ANIMATION_EFFECT_BLUR,
					   ANIMATION_EFFECT_FLIP,
					   ANIMATION_EFFECT_SCRAMBLE};
	const char *flag_names[] = {"None", "Fold", "Mirror", "Loop", "Cycle", "Shift", "Blur", "Flip", "Scramble"};
	const int effectCount = 9;
	for (int i = 0; i < effectCount; i++)
	{
		if (flags[i] == ambient.params.flags_)
		{
			flags_idx = i;
			break;
		}
	}
	flags_idx = (flags_idx + effectCount + (next ? 1 : -1)) % effectCount;
	uart_get_service().printf("P: flag V: %s\n", flag_names[flags_idx]);
	ambient.params.flags_ = flags[flags_idx];
}

void maskCycle(bool next, uint8_t unused)
{
	uint8_t mask = ambient.params.mask_;
	mask = (mask + 256 + (next ? 1 : -1)) % 256;
	char buf[32];
	getMaskName(buf, mask);
	uart_get_service().printf("P: mask V: %s\n", buf);
	ambient.params.mask_ = mask;
}

void filterCycle(bool next, uint8_t unused)
{
	uint8_t filter = ambient.params.filter_;
	filter = (filter + 256 + (next ? 1 : -1)) % 256;
	char buf[32];
	getFilterName(buf, filter);
	uart_get_service().printf("P: filter V: %s\n", buf);
	ambient.params.filter_ = filter;
}

void animCycle(bool next, uint8_t unused)
{
	uint8_t anim_idx = (uint8_t)ambient.name;
	anim_idx = (anim_idx + get_animations_count() + (next ? 1 : -1)) % get_animations_count();
	uart_get_service().printf("P:anim V:%.10s\n", get_animation_name((Anim)anim_idx));
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
	uart_get_service().printf("P: mode V: %s\n", get_config_mode_name(modes[mode_idx]));
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
	uart_get_service().printf("P: power V: %d\n", power);
	config.ad.power = power;
}

void decayCycle(bool next, uint8_t unused)
{
	uint8_t decay = config.ad.decay;
	decay = (decay + 255 + (next ? 16 : -16)) % 255;
	uart_get_service().printf("P: decay V: %4.2f%%\n", (float)decay * 100.f / 255.f);
	config.ad.decay = decay;
}

void rangeCycle(bool next, uint8_t unused)
{
	int8_t range = config.ad.range;
	range = max(-120, min(0, range + (next ? -2 : 2)));
	uart_get_service().printf("P: range V: %d\n", range);
	config.ad.range = range;
}

void animRSSICycle(bool next, uint8_t unused)
{
	ambient.params.flags_ ^= ANIMATION_FLAG_USE_SIGNAL_POWER;
	uart_get_service().printf("P: anim rssi V: %d\n", ambient.params.flags_ & ANIMATION_FLAG_USE_SIGNAL_POWER);
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
uint8_t g_AnimIdx = 0;
uint8_t g_ConfigIdx = 0;
bool g_tweaker_is_modifying_animation = true;

bool g_ble_uart_packet_in_progress = false;

void prph_bleuart_rx_callback(uint16_t conn_handle)
{
	(void)conn_handle;

	// Wait for new data to arrive
#define MAX_BLE_UART_PACKET 200
	static char str[MAX_BLE_UART_PACKET] = {0};
	static uint8_t len = 0;

	int avail = uart_get_service().available();
	if (avail <= 0)
	{
		return;
	}

	if (avail + len > MAX_BLE_UART_PACKET)
	{
		Serial.println("ERROR: Buffer Overflow");
	}

	if (g_ble_uart_packet_in_progress)
	{
		if (uart_get_service().peek() == '!')
		{
			LOG_LV1("BLE", "Old packet incomplete, starting new one.");
			memset(str, 0, MAX_BLE_UART_PACKET);
			len = uart_get_service().read(str, avail);
		}
		else
		{
			len += uart_get_service().read(str + strlen(str), avail);
		}
	}
	else
	{
		memset(str, 0, MAX_BLE_UART_PACKET);
		len = uart_get_service().read(str, avail);
	}

	// Serial.print("[Prph] RX: ");
	// Serial.println(str);

	if (str[0] != '!')
	{
		Serial.println("uart_get_service() service got rogue packet");
		Serial.printBuffer(str, len);
		return;
	}

	// StartupConfig
	if (str[1] == 'S')
	{
		if (str[strlen(str) - 1] != '#')
		{
			g_ble_uart_packet_in_progress = true;
			return;
		}
		g_ble_uart_packet_in_progress = false;
		char *configStr = &str[2];

		StartupConfig config = deserializeStartupConfig(configStr, len - 2);
		Serial.println("Setting startup config from BLE service");
		Serial.printf("%s", configStr);
		localSettings_.startupConfig_ = config;
		write_local_settings();
		startWithConfig(config);
		return;
	}

	// Set Ambient Animation
	if (str[1] == 'A')
	{
		if (str[strlen(str) - 1] != '#')
		{
			g_ble_uart_packet_in_progress = true;
			return;
		}
		g_ble_uart_packet_in_progress = false;

		char *animStr = &str[2];
		animPattern pattern = deserializeAnimPattern(animStr, len - 2);
		Serial.println("Setting animation pattern from BLE service");
		Serial.printf("%s", animStr);
		localSettings_.startupConfig_.pattern = pattern;
		//write_local_settings();
		led_set_ambient_animation(pattern);
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
				uart_get_service().printf("P: %s\n", g_AnimCyclerNames[g_AnimIdx]);
			}
			else
			{
				g_ConfigIdx = (g_ConfigIdx + 4) % 5;
				uart_get_service().printf("P: %s\n", g_ConfigCyclerNames[g_ConfigIdx]);
			}
		}
		else if (button == '6') // down
		{
			if (g_tweaker_is_modifying_animation)
			{
				g_AnimIdx = (g_AnimIdx + 1) % 9;
				uart_get_service().printf("P: %s\n", g_AnimCyclerNames[g_AnimIdx]);
			}
			else
			{
				g_ConfigIdx = (g_ConfigIdx + 1) % 5;
				uart_get_service().printf("P: %s\n", g_ConfigCyclerNames[g_ConfigIdx]);
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
				uart_get_service().printf("Anim: %s\n", get_animation_name(ambient.name));
			}
			else
			{
				uart_get_service().printf("Conf: %s\n", get_config_mode_name(config.mode));
			}
		}
		else if (button == '2') // The Info Details Button
		{
			// Turn off the harsh blue light for animation configuration
			digitalWrite(LED_BUILTIN, !LED_STATE_ON);
			if (g_tweaker_is_modifying_animation)
			{
				uart_get_service().printf("mask: %d filter: %d\n", ambient.params.mask_, ambient.params.filter_);
			}
			else
			{
				uart_get_service().printf(":P\n");
			}
		}
		else if (button == '3') // The mode switcher button
		{
			if (g_tweaker_is_modifying_animation)
			{
				uart_get_service().printf("Editing config\n");
				g_tweaker_is_modifying_animation = false;
			}
			else
			{
				uart_get_service().printf("Editing anim\n");
				g_tweaker_is_modifying_animation = true;
			}
		}
		else if (button == '4') // Commit the changes button (after system reset)
		{
			uart_get_service().printf("Saving Config\n");
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
			uart_get_service().printf("Restarting\n");
			NVIC_SystemReset();
		}

		// Set the ambient animation
		led_set_ambient_animation(ambient);
		return;
	}

	Serial.print("[Prph] Unknown BLE Uart command RX: ");
	Serial.println(str);
}