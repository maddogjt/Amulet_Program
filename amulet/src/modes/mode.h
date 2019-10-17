#pragma once

typedef enum
{
	AMULET_MODE_FIRSTBOOT,
	AMULET_MODE_BURN,
	AMULET_MODE_SIMON,
	AMULET_MODE_SEACOMP_AMULET,
	AMULET_MODE_SAFE,
	AMULET_MODE_PHOTOKEY,
	AMULET_MODE_COUNT,
} amulet_mode_t;

const char *get_config_mode_name(amulet_mode_t mode);

class AmuletMode {
public:
	virtual void start(){}
	virtual void loop();
	virtual void buttonPressMode();
	virtual void buttonPressReset();
	virtual void buttonHoldMode(){}
	virtual void buttonHoldReset(){}
};

AmuletMode *amulet_mode_get();
AmuletMode *amulet_mode_start(amulet_mode_t mode, bool enterConfig);
void amulet_mode_loop();