
#include "signal.h"

#include <Arduino.h>

#define ARDUINO_GENERIC
#include <FastLED.h>
#undef ARDUINO_GENERIC

#include "Startup.h"
#include "settings.h"

Signal *topSignal = nullptr;

// Precise method, which guarantees v = v1 when t = 1.
float lerp(float v0, float v1, float t)
{
	return (1 - t) * v0 + t * v1;
}

void insert_new_scan(Scan &s);
void add_scan_data(Scan &s)
{
	if (mode == AMULET_MODE_RUNE && s.signal_type != (uint8_t)AdvertisementType::Amulet)
	{
		// Runes only care about amulets;
		return;
	}

	if (isAmulet() && s.signal_type == (uint8_t)AdvertisementType::Amulet)
	{
		// Amulets do not care about amulets;
		return;
	}

	if (topSignal != nullptr && topSignal->_scan == s)
	{
		// Update the top signal
		topSignal->_nextStrength = max(topSignal->_nextStrength, s.power);
		topSignal->_seenCount = topSignal->_seenCount + 1;
		topSignal->_scan.rssi = s.rssi;
		return;
	}

	int topRunePower = topSignal != nullptr ? topSignal->_strength : 0;
	if (s.power > topRunePower)
	{
		// New top signal
		if (topSignal != nullptr)
		{
			delete topSignal;
			topSignal = nullptr;
		}
		topSignal = new Signal(s);
		topSignal->_seenCount = 1;
		LOG_LV1("SIGNAL", "New top signal: %s", get_advertisement_type_name((AdvertisementType)topSignal->_scan.signal_type));
		LOG_LV1("SIGNAL", "    Power: %d, RSSI: %d", topSignal->_scan.power, topSignal->_scan.rssi);
	}
}

void decay_signals()
{
	if (topSignal == nullptr)
	{
		return;
	}

	if (topSignal->_nextStrength > topSignal->_strength)
	{
		// LOG_LV3("SIG", "decay_signals : New strength was equal %f -> %f", topSignal->_strength, topSignal->_nextStrength);
	}
	if (topSignal->_nextStrength > topSignal->_strength)
	{
		// LOG_LV3("SIG", "decay_signals : New strength was greater %f -> %f", topSignal->_strength, topSignal->_nextStrength);
		topSignal->_strength = topSignal->_nextStrength;
	}
	else
	{
		float_t decay = min(1.f, max(0.05, topSignal->_scan.decayRate));
		// LOG_LV3("SIG", "decay_signals : Decaying lerp( %f, %f %f ) -> %f", topSignal->_strength, topSignal->_nextStrength, decay, lerp(topSignal->_strength, topSignal->_nextStrength, decay));
		topSignal->_strength = lerp(topSignal->_strength, topSignal->_nextStrength, decay);
	}

	// If the signal is too weak, cull it.
	if (topSignal->_strength <= globalSettings_.ambientPowerThreshold_)
	{
		// LOG_LV3("SIG", "decay_signals :Culling strength less than %d", globalSettings_.ambientPowerThreshold_);
		delete topSignal;
		topSignal = nullptr;
		LOG_LV1("SIGNAL", "New top signal: NULL (decay)");
	}
	else
	{
		topSignal->_nextStrength = 0;
	}
}

void signal_loop(int step)
{
	if (isAmulet() || mode == AMULET_MODE_RUNE)
	{
		EVERY_N_SECONDS(1)
		{
			decay_signals();
		}
	}
}

Signal *current_top_signal()
{
	return topSignal;
}
