#pragma once

typedef enum
{
	AMULET_MODE_FIRSTBOOT,
	AMULET_MODE_BLINKY,
	AMULET_MODE_RUNE,
	AMULET_MODE_BEACON,
	AMULET_MODE_LAMP,
	AMULET_MODE_COUNT,
} amulet_mode_t;

const char *get_config_mode_name(amulet_mode_t mode);

typedef enum
{
	// Received at the start of a press
	BUTTON_ACTION_PRESS,
	// Received when a button press has become a long press
	BUTTON_ACTION_LONG_PRESS_START,
	
	// Received when a button is released, but not if it's been a long pressed
	BUTTON_ACTION_RELEASE,

	// Received when a long press is released
	BUTTON_ACTION_LONG_PRESS_RELEASE
} button_action_t;

class AmuletMode {
public:
	virtual void start(){}
	virtual void loop();
	virtual void buttonActionMode(button_action_t action);
	virtual void buttonActionReset(button_action_t action);
};

AmuletMode *amulet_mode_get();
AmuletMode *amulet_mode_start(amulet_mode_t mode, bool enterConfig);
void amulet_mode_loop();