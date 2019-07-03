#pragma once

#include <Arduino.h>
#include "ble.h"

struct Scan
{
	int8_t rssi;
	uint8_t signal_type;
	uint8_t power;
	float_t decayRate;
	uint8_t _data[MAX_MFD_DATA_LEN];
	inline void setData(const uint8_t *buffer)
	{
		memcpy(_data, buffer, MAX_MFD_DATA_LEN);
	};
	// Note: equality operator DOES NOT compare rssi.
	bool operator==(const Scan &s) const
	{
		bool dataMatches = !memcmp(_data, s._data, MAX_MFD_DATA_LEN);
		return dataMatches && signal_type == s.signal_type && power == s.power && decayRate == s.decayRate;
	};
};

class Signal
{
public:
	Signal(Scan &scan) : _scan(scan), _strength(scan.power), _nextStrength(0) {}
	Scan _scan;
	float_t _strength;
	float_t _nextStrength;
};

void signal_loop(int step);
void add_scan_data(Scan &s);
Signal *current_top_signal();
