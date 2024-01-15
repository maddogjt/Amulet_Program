#include "configuration.h"
#include "../animation/animations.h"
#include "../animation/animation_modifiers.h"
#include "../animation/animation_overlay.h"
#include "../communication/uart.h"
#include "../leds/led.h"
#include "../settings/dev_mode.h"
#include "../settings/settings.h"
#include "bluefruit.h"
#include <ArduinoJson.h>

//#include "../../signal.h"

static uint8_t g_AnimIdx = 0;
static uint8_t g_ConfigIdx = 0;
static bool g_tweaker_is_modifying_animation = true;

StartupConfig &getConfig()
{
	return localSettings_.startupConfig_;
}

anim_config_t &getAnim()
{
	return getConfig().getConfigModeAnim(getConfig().mode);
}

class ParamEditor
{
	virtual int32_t getValue() const = 0;
	virtual void setValue(int32_t value) = 0;
	virtual int32_t getNext(bool next) const = 0;
};

template <typename TWriter>
class BufferedWriter
{
public:
	explicit BufferedWriter(TWriter &writer, size_t bufferSize) : _writer(writer), _bufferSize(min(bufferSize, 256)), _count(0) {}

	size_t write(uint8_t c)
	{
		return write(&c, 1);
	}

	size_t write(const uint8_t *s, size_t n)
	{
		size_t bufRemain = _bufferSize - _count;
		size_t dataRemain = n;
		const uint8_t *src = s;
		while (dataRemain)
		{
			auto toCopy = min(dataRemain, _bufferSize - _count);
			memcpy(_buffer + _count, src, toCopy);
			src += toCopy;
			dataRemain -= toCopy;
			_count += toCopy;
			if (_count == _bufferSize)
			{
				flush();
			}
		}

		return src - s;
		// _count += _writer.write(s, n);
	}

	void flush()
	{
		if (_count > 0)
		{
			_writer.write(_buffer, _count);
			_count = 0;
		}
	}

private:
	byte _buffer[256];
	TWriter _writer;
	size_t _bufferSize;
	size_t _count;
};

void writeConfig()
{
	DynamicJsonDocument doc(2000);
	BufferedWriter<Stream &> buffer(uart_stream(), Bluefruit.getMaxMtu(BLE_GAP_ROLE_PERIPH) - 3);
	{
		doc["devMode"] = dev_mode_enabled();
		auto &data = getAnim();
		auto anim = doc.createNestedObject("anim");

		anim["name"] = animation_get_name(data.anim_);
		anim["flag"] = animation_modifier_get_name(data.modifiers_);
		anim["mask"] = animation_get_name(data.overlay_);
		anim["filter"] = animation_overlay_get_filter_name(data.filter_);
		anim["color1"] = data.color1_;
		anim["color2"] = data.color2_;
		anim["speed"] = data.speed_;
		anim["extra0"] = data.extra0_;
		anim["extra1"] = data.extra1_;

		// 	speedCycle,
		// 	flagCycle,
		// 	maskCycle,
		// 	filterCycle,
		// 	extraCycle,
		// 	extraCycle,
	}
	{
		auto &data = getConfig();
		auto config = doc.createNestedObject("config");
		config["mode"] = get_config_mode_name(data.mode);
		auto adv = config.createNestedObject("adv");
		adv["power"] = data.ad.power;
		adv["decay"] = data.ad.decay;
		adv["range"] = data.ad.range;
		adv["group"] = data.ad.groupId;
		config["intLedEn"] = data.intLedEn_;
		config["extLedExtend"] = data.externalLedExtend_;
		config["extLedCount"] = data.externalLedCount_;

		config["extLeds"] = data.externalLedEn_;
		auto bright = config.createNestedArray("bright");
		bright.add(data.brightness_[0]);
		bright.add(data.brightness_[1]);
		bright.add(data.brightness_[2]);
		auto extBright = config.createNestedArray("extBright");
		extBright.add(data.extBrightness_[0]);
		extBright.add(data.extBrightness_[1]);
		extBright.add(data.extBrightness_[2]);
	}

	// BufferedWriter<Stream> &b = buffer;
	serializeJson(doc, buffer);
	buffer.flush();
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
	if (next)
	{
		return (value + 1) % count;
	}
	else
	{
		return value > 0 ? (value - 1) : (count - 1);
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

class ModeCycle : public ParamEditor
{
	int32_t getValue() const { return getConfig().mode; }
	void setValue(int32_t value)
	{
		uint8_t mode_idx = 0;

		auto &config = getConfig();
		config.mode = (amulet_mode_t)value;

		if (config.mode == AMULET_MODE_BURN)
		{
			config.ad.power = 150;
			config.ad.decay = 96;
			config.ad.range = -90;
		}
		// else if (config.mode == AMULET_MODE_RUNE)
		// {
		// 	config.ad.power = 150;
		// 	config.ad.decay = 96;
		// 	config.ad.range = -90;
		// }
		// else if (config.mode == AMULET_MODE_BEACON_POWER_AMULET)
		// {
		// 	config.ad.power = 180;
		// 	config.ad.decay = 160;
		// 	config.ad.range = -70;
		// }
		// else if (config.mode == AMULET_MODE_AMULET)
		// {
		// 	config.ad.power = 80;
		// 	config.ad.decay = 64;
		// 	config.ad.range = -70;
		// }
	}
	int32_t getNext(bool next) const
	{
		int32_t value = getValue();

		value = (value + (next ? 1 : (int)AMULET_MODE_COUNT + 1)) % (int)AMULET_MODE_COUNT;
		uart_stream().printf("P: mode V: %s\n", get_config_mode_name((amulet_mode_t)value));
		return value;
	}
};

void modeCycle(bool next, uint8_t unused)
{
	int currentMode = (int)getConfig().mode;
	uint8_t mode_idx = 0;

	currentMode = (currentMode + (next ? 1 : (int)AMULET_MODE_COUNT + 1)) % (int)AMULET_MODE_COUNT;
	uart_stream().printf("P: mode V: %s\n", get_config_mode_name((amulet_mode_t)currentMode));
	auto &config = getConfig();
	config.mode = (amulet_mode_t)currentMode;

	if (config.mode == AMULET_MODE_BURN)
	{
		config.ad.power = 150;
		config.ad.decay = 96;
		config.ad.range = -90;
	}
	// else if (config.mode == AMULET_MODE_RUNE)
	// {
	// 	config.ad.power = 150;
	// 	config.ad.decay = 96;
	// 	config.ad.range = -90;
	// }
	// else if (config.mode == AMULET_MODE_BEACON_POWER_AMULET)
	// {
	// 	config.ad.power = 180;
	// 	config.ad.decay = 160;
	// 	config.ad.range = -70;
	// }
	// else if (config.mode == AMULET_MODE_AMULET)
	// {
	// 	config.ad.power = 80;
	// 	config.ad.decay = 64;
	// 	config.ad.range = -70;
	// }
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

void groupIdCycle(bool next, uint8_t unused)
{
	uint8_t value = getConfig().ad.groupId;
	value = (value + 255 + (next ? 1 : -1)) % 255;
	uart_stream().printf("P: group V: %d\n", value);
	getConfig().ad.groupId = value;
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

void extLedsModeCycle(bool next, uint8_t unused)
{
	localSettings_.startupConfig_.externalLedEn_ = !localSettings_.startupConfig_.externalLedEn_;
	uart_stream().printf("P: extLedEn V: %s\n", localSettings_.startupConfig_.externalLedEn_ ? "true" : "false");
}
void intLedEnCycle(bool next, uint8_t unused)
{
	localSettings_.startupConfig_.intLedEn_ = !localSettings_.startupConfig_.intLedEn_;
	uart_stream().printf("P: intLedEn V: %s\n", localSettings_.startupConfig_.intLedEn_ ? "true" : "false");
}

	void extLedExtendCycle(bool next, uint8_t unused)
{
	localSettings_.startupConfig_.externalLedExtend_ = !localSettings_.startupConfig_.externalLedExtend_;
	uart_stream().printf("P: extLedExtend V: %s\n", localSettings_.startupConfig_.externalLedExtend_ ? "true" : "false");
}

void extLedCountCycle(bool next, uint8_t unused)
{
	int increment = (next ? 1 : -1);
	localSettings_.startupConfig_.externalLedCount_ += EXTERNAL_LED_MAX_COUNT + increment;
	// safety check
	localSettings_.startupConfig_.externalLedCount_ = localSettings_.startupConfig_.externalLedCount_ % EXTERNAL_LED_MAX_COUNT;
	uart_stream().printf("P: extLedCount V: %d\n", localSettings_.startupConfig_.externalLedCount_);
}

void brightCycle(int index, bool next)
{
	int increment = (next ? 4 : -4);
	localSettings_.startupConfig_.brightness_[index] += increment;
	// safety check
	localSettings_.startupConfig_.brightness_[index] = localSettings_.startupConfig_.brightness_[index] % 128;
	uart_stream().printf("P: b%d V: %d\n", index, localSettings_.startupConfig_.brightness_[index]);
	led_refresh_brightness();
}

void extBrightCycle(int index, bool next)
{
	int increment = (next ? 4 : -4);
	localSettings_.startupConfig_.extBrightness_[index] += increment;
	// safety check
	localSettings_.startupConfig_.extBrightness_[index] = localSettings_.startupConfig_.extBrightness_[index] % 128;
	uart_stream().printf("P: eb%d V: %d\n", index, localSettings_.startupConfig_.extBrightness_[index]);
	led_refresh_brightness();
}

void brightness0Cycle(bool next, uint8_t unused)
{
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

void extBrightness0Cycle(bool next, uint8_t unused)
{
	extBrightCycle(0, next);
}

void extBrightness1Cycle(bool next, uint8_t unused)
{
	extBrightCycle(1, next);
}

void extBrightness2Cycle(bool next, uint8_t unused)
{
	extBrightCycle(2, next);
}

typedef void (*ParameterCycleList[])(bool next, uint8_t idx);
ParameterCycleList g_ConfigCyclers = {
	modeCycle,
	powerCycle,
	groupIdCycle,
	decayCycle,
	rangeCycle,
	animRSSICycle,
	extLedsModeCycle,
	extLedExtendCycle,
	extLedCountCycle,
	intLedEnCycle,
	brightness0Cycle,
	brightness1Cycle,
	brightness2Cycle,
	extBrightness0Cycle,
	extBrightness1Cycle,
	extBrightness2Cycle,
};
constexpr auto kConfigCount = sizeof(g_ConfigCyclers) / sizeof(g_ConfigCyclers[0]);

const char *g_ConfigCyclerNames[] = {
	"mode",
	"power",
	"groupId",
	"decay",
	"range",
	"AnimRSSI",
	"extLedMode",
	"extLedExtend",
	"extLedCount",
	"intLedEn",
	"bright0",
	"bright1",
	"bright2",
	"extBright0",
	"extBright1",
	"extBright2",
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
constexpr auto kAnimCount = sizeof(g_AnimCyclers) / sizeof(g_AnimCyclers[0]);

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
		Serial.printf("uart_stream() service got rogue packet (%db)\n", len);
		Serial.printBuffer(str, len);
		Serial.println("done");
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
		start_animation(getAnim());
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
				g_AnimIdx = min(g_AnimIdx - 1, kAnimCount - 1);
				uart_stream().printf("P: %s\n", g_AnimCyclerNames[g_AnimIdx]);
			}
			else
			{
				g_ConfigIdx = min(g_ConfigIdx - 1, kConfigCount - 1);
				uart_stream().printf("P: %s\n", g_ConfigCyclerNames[g_ConfigIdx]);
			}
		}
		else if (button == '6') // down
		{
			if (g_tweaker_is_modifying_animation)
			{
				g_AnimIdx = (g_AnimIdx + 1) % kAnimCount;
				uart_stream().printf("P: %s\n", g_AnimCyclerNames[g_AnimIdx]);
			}
			else
			{
				g_ConfigIdx = (g_ConfigIdx + 1) % kConfigCount;
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

			writeConfig();
			// uart_stream().printf("How about a longer stream.  How well will this work?");
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
			localSettings_.startupConfig_.enterConfigMode_ = false;

			write_local_settings();

			delay(500);
			uart_stream().printf("Restarting\n");
			NVIC_SystemReset();
		} else if (button == '9') {
			Serial.println("toggling dev mode");
			toggle_dev_mode_enabled_and_reboot();
		}

		// Set the ambient animation
		auto &anim = localSettings_.startupConfig_.getConfigModeAnim(localSettings_.startupConfig_.mode);
		start_animation(anim);
		return;
	}

	Serial.print("[Prph] Unknown BLE Uart command RX: ");
	Serial.println(str);
}