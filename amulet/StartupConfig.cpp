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
	case AMULET_MODE_COUNT:
	default:
		break;
	}
	return "err";
}

StartupConfig defaultConfigForRemoteSetup()
{
	StartupConfig c{};
	c.mode = AMULET_MODE_BEACON;
	c.ad.power = 80;
	c.ad.decay = 128;
	c.ad.range = -80;
	c.pattern.anim_ = Anim::AnimPrism;
	return c;
}