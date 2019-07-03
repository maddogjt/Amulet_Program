#pragma once

#include <Arduino.h>

enum class AdvertisementType
{
	Unknown,
	Amulet,
	Beacon,
	Rune,
	Special,
};

#define MAX_MFD_DATA_LEN 12
extern int g_rssi;

void ble_setup_uart();
void ble_setup(bool advertise, bool scan);
void ble_set_advertisement_data(const AdvertisementType type, const uint8_t *data, const uint8_t len);
