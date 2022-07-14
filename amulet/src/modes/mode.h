#pragma once

typedef enum
{
	AMULET_MODE_FIRSTBOOT,
	AMULET_MODE_BLINKY,
	AMULET_MODE_RUNE,
	AMULET_MODE_BEACON,
	AMULET_MODE_COUNT,
} amulet_mode_t;

const char *get_config_mode_name(amulet_mode_t mode);

class AmuletMode {
public:
	virtual void start(){}
	virtual void loop();
	virtual void buttonPressMode(bool released);
	virtual void buttonPressReset(bool released);
	virtual void buttonHoldMode(bool released){}
	virtual void buttonHoldReset(bool released){}
};

AmuletMode *amulet_mode_get();
AmuletMode *amulet_mode_start(amulet_mode_t mode, bool enterConfig);
void amulet_mode_loop();