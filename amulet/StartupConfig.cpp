#include "StartupConfig.h"

int first_tok(char *str)
{
	return atoi(strtok(str, ","));
}
int next_tok()
{
	char *pch = strtok(NULL, ",");
	if (pch)
	{
		return atoi(pch);
	}
	else
	{
		LOG_LV1("CONFIG", "ERROR: Expected valid token, got null");
		return 0;
	}
}

StartupConfig deserializeStartupConfig(char *str, uint8_t len)
{
	StartupConfig config;
	config.mode = (amulet_mode_t)first_tok(str);
	config.power = next_tok();
	config.decay = next_tok();
	config.range = next_tok();
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

void append_uint8_t(char *str, int x)
{
}

// returns len
uint8_t serializeStartupConfig(char *buffer, const uint8_t len, const StartupConfig &config)
{
	memset(buffer, 0, len);

#define APPEND_INT_COMMA(X)                    \
	itoa((int)X, buffer + strlen(buffer), 10); \
	strcat(buffer, ",");

	APPEND_INT_COMMA(config.mode)
	APPEND_INT_COMMA(config.power)
	APPEND_INT_COMMA(config.decay)
	APPEND_INT_COMMA(config.range)
	APPEND_INT_COMMA(config.pattern.name)
	APPEND_INT_COMMA(config.pattern.params.color1_)
	APPEND_INT_COMMA(config.pattern.params.color2_)
	APPEND_INT_COMMA(config.pattern.params.speed_)
	APPEND_INT_COMMA(config.pattern.params.flags_)
	APPEND_INT_COMMA(config.pattern.params.mods_)
	APPEND_INT_COMMA(config.pattern.params.extra0_)
	APPEND_INT_COMMA(config.pattern.params.extra1_)
	buffer[strlen(buffer) - 1] = NULL; // delete last comma

	Serial.println(buffer);
	Serial.printf("Buffer (%d): %s\n", strlen(buffer), buffer);
	return strlen(buffer);
}

void testSerialization()
{
	StartupConfig config{};
	config.mode = AMULET_MODE_BEACON;
	config.power = 80;
	config.decay = 50;
	config.range = -80;
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
