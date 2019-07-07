
#include "signal.h"

#include <Arduino.h>

#define ARDUINO_GENERIC
#include <FastLED.h>
#undef ARDUINO_GENERIC

#include "Startup.h"
#include "settings.h"

Signal *topSignal;

// Precise method, which guarantees v = v1 when t = 1.
float lerp(float v0, float v1, float t)
{
	return (1 - t) * v0 + t * v1;
}

void insert_new_scan(Scan &s);
void add_scan_data(Scan &s)
{
	LOG_LV1("SIG", "add_scan_data");
	int topRunePower = topSignal != nullptr ? topSignal->_strength : 0;

	if (s.power > topRunePower)
	{
		if (topSignal != nullptr)
		{
			delete topSignal;
			topSignal = nullptr;
		}
		topSignal = new Signal(s);
		topSignal->_seenCount = 1;
	}
}

void decay_signals()
{
	LOG_LV1("SIG", "decay_signals");
	if (topSignal == nullptr)
	{
		return;
	}

	if (topSignal->_nextStrength > topSignal->_strength)
	{
		LOG_LV1("SIG", "decay_signals : New strength was equal %f -> %f", topSignal->_strength, topSignal->_nextStrength);
	}
	if (topSignal->_nextStrength > topSignal->_strength)
	{
		LOG_LV1("SIG", "decay_signals : New strength was greater %f -> %f", topSignal->_strength, topSignal->_nextStrength);
		topSignal->_strength = topSignal->_nextStrength;
	}
	else
	{
		float_t decay = min(1.f, max(0.05, topSignal->_scan.decayRate));
		LOG_LV1("SIG", "decay_signals : Decaying lerp( %f, %f %f ) -> %f", topSignal->_strength, topSignal->_nextStrength, decay, lerp(topSignal->_strength, topSignal->_nextStrength, decay));
		topSignal->_strength = lerp(topSignal->_strength, topSignal->_nextStrength, decay);
	}

	// If the signal is too weak, cull it.
	if (topSignal->_strength <= globalSettings_.ambientPowerThreshold_)
	{
		LOG_LV1("SIG", "decay_signals :Culling strength less than %d", globalSettings_.ambientPowerThreshold_);
		delete topSignal;
		topSignal = nullptr;
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
		decay_signals();
	}
}

Signal *current_top_signal()
{
	return topSignal;
}
