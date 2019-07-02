

typedef enum
{
	AMULET_MODE_STARTUP,
	AMULET_MODE_AMULET,
	AMULET_MODE_BEACON,
	AMULET_MODE_RUNE,
	AMULET_MODE_POWER_AMULET,
	AMULET_MODE_COUNT,
} amulet_mode_t;

extern amulet_mode_t mode;

void start();