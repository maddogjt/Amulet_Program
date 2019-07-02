#pragma once

#include <Arduino.h>

typedef struct ATTR_PACKED
{
	uint16_t company_id;
	uint8_t command;
	uint8_t param0;
	uint8_t param1;
	uint8_t param2;
} amulet_mfd_t;

extern int g_rssi;

void ble_setup_uart();
void ble_setup(bool advertise, bool scan);
void ble_loop(int step);
void ble_set_advertisement_data(amulet_mfd_t &data);
void ble_viral_override(amulet_mfd_t &override);
