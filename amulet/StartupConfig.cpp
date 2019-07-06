#include "StartupConfig.h"

#include "CSVHelpers.hpp"

const char *get_config_mode_name(amulet_mode_t mode)
{
	switch (mode)
	{
	case AMULET_MODE_CONFIG:
		return "config";
		break;
	case AMULET_MODE_AMULET:
		return "amulet";
		break;
	case AMULET_MODE_BEACON:
		return "camp";
		break;
	case AMULET_MODE_RUNE:
		return "rune";
		break;
	case AMULET_MODE_BEACON_POWER_AMULET:
		return "power";
		break;
	case AMULET_MODE_RUNIC_POWER_AMULET:
		return "runulet";
		break;
	case AMULET_MODE_COUNT:
	default:
		break;
	}
	return "err";
}

StartupConfig deserializeStartupConfig(char *str, uint8_t len)
{
	StartupConfig config;
	config.mode = (amulet_mode_t)first_tok(str);
	config.ad.power = next_tok();
	config.ad.decay = next_tok();
	config.ad.range = next_tok();
	config.pattern.name = (Anim)next_tok();
	config.pattern.params.color1_ = next_tok();
	config.pattern.params.color2_ = next_tok();
	config.pattern.params.speed_ = next_tok();
	config.pattern.params.flags_ = next_tok();
	config.pattern.params.mods_ = next_tok();
	config.pattern.params.extra0_ = next_tok();
	config.pattern.params.extra1_ = next_tok();
	return config;
}

// returns len
uint8_t serializeStartupConfig(char *buffer, const uint8_t len, const StartupConfig &config)
{
	memset(buffer, 0, len);

#define APPEND_INT_COMMA(X)                    \
	itoa((int)X, buffer + strlen(buffer), 10); \
	strcat(buffer, ",");

	APPEND_INT_COMMA(config.mode)
	APPEND_INT_COMMA(config.ad.power)
	APPEND_INT_COMMA(config.ad.decay)
	APPEND_INT_COMMA(config.ad.range)
	APPEND_INT_COMMA(config.pattern.name)
	APPEND_INT_COMMA(config.pattern.params.color1_)
	APPEND_INT_COMMA(config.pattern.params.color2_)
	APPEND_INT_COMMA(config.pattern.params.speed_)
	APPEND_INT_COMMA(config.pattern.params.flags_)
	APPEND_INT_COMMA(config.pattern.params.mods_)
	APPEND_INT_COMMA(config.pattern.params.extra0_)
	APPEND_INT_COMMA(config.pattern.params.extra1_)
	buffer[strlen(buffer) - 1] = '\0'; // delete last comma

	Serial.println(buffer);
	Serial.printf("Buffer (%d): %s\n", strlen(buffer), buffer);
	return strlen(buffer);
}

void testSerialization()
{
	StartupConfig config{};
	config.mode = AMULET_MODE_BEACON;
	config.ad.power = 80;
	config.ad.decay = 50;
	config.ad.range = -80;
	config.pattern.name = Anim::AnimSinelon;
	config.pattern.params.color1_ = 40;
	config.pattern.params.mods_ = 1;

	char buffer[200];
	uint8_t len = serializeStartupConfig(buffer, 200, config);

	StartupConfig config2 = deserializeStartupConfig(buffer, len);

	if (config == config2)
	{
		Serial.println("SUCCESS: Serialization test passed!");
	}
	else
	{
		Serial.println("ERROR: Serialization test failed!");
	}
}
