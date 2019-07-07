#pragma once

#include <Arduino.h>

enum class AdvertisementType
{
	Unknown,
	Amulet,  // Lowest priority, doesn't set things
	Beacon,  // Temporarily sets pattern on amulets
	Runic,   // Permanently sets pattern on amulet
	Special, // undefined
};

const char *get_advertisement_type_name(AdvertisementType adType);

struct advertisementParams
{
	uint8_t power;
	uint8_t decay; // This is treated as a fract8, ie the uint value will be divided by 256 to get a floating point value 0..1
	int8_t range;
	bool operator==(const advertisementParams &ap) const
	{
		return power == ap.power && decay == ap.decay && range == ap.range;
	}
};

#define MAX_MFD_DATA_LEN 12
extern int g_rssi;

void ble_setup_uart();
void ble_setup(bool advertise, bool scan, bool uart = false);
void ble_set_advertisement_data(const AdvertisementType type, const advertisementParams &params, const uint8_t *data, const uint8_t len);
void ble_loop();