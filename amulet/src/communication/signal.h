#pragma once

#include "packet_types.h"

class Signal
{
public:
	Signal(int8_t rssi, const amulet_mfg_data_t data, float strength)
		: data_(data),
		  rssi_(rssi),
		  strength_(strength),
		  signalStrength_(strength),
		  seenCount_(1) {}
	//	Scan _scan;
	float_t strength_;
	float_t signalStrength_;
	int seenCount_;
	amulet_mfg_data_t data_;
	int8_t rssi_;
};

void signal_loop(int step);
void set_mimic_mode(bool mimic);
void signal_add_scan_data(int8_t rssi, const amulet_mfg_data_t &data);
Signal *signal_get_current_top();
