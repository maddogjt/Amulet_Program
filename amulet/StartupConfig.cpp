#include "StartupConfig.h"

void deserializeAnimPacket(animPattern &pattern, char *str, uint8_t len)
{
	if (len != 12)
	{
		LOG_LV1("CONF", "ERROR: Bad length in animation packet");
		return;
	}
	// uint8_t name = str[0];
	// pattern.params.color1_ = str[1];
	// pattern.params.color1_ = str[2];
	// pattern.params.color1_ = str[3];
	// pattern.params.color1_ = str[4];
	// pattern.params.color1_ = str[5];
	// pattern.params.color1_ = str[6];
	// pattern.params.color1_ = str[7];
}
void deserializeModeAndSignal(StartupConfig &config, char *buffer, uint8_t len)
{
}
