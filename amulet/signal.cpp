
#include "signal.h"

#include <Arduino.h>

#define ARDUINO_GENERIC
#include <FastLED.h>
#undef ARDUINO_GENERIC

#include "Startup.h"

#define SIGNALS_MAX_LEN 20
Signal *signals[SIGNALS_MAX_LEN];

// Precise method, which guarantees v = v1 when t = 1.
float lerp(float v0, float v1, float t)
{
	return (1 - t) * v0 + t * v1;
}

void insert_new_scan(Scan &s);
void add_scan_data(Scan &s)
{
	LOG_LV1("SIG", "add_scan_data");
	bool found = false;
	for (int i = 0; i < SIGNALS_MAX_LEN; i++)
	{
		if (signals[i] == nullptr)
		{
			continue;
		}
		if (signals[i]->_scan == s)
		{
			LOG_LV1("SIG", "add_scan_data : Updating existing record");
			signals[i]->_nextStrength = max(signals[i]->_nextStrength, s.power);
			found = true;
			break;
		}
	}
	if (!found)
	{
		insert_new_scan(s);
	}
}

void insert_new_scan(Scan &s)
{
	LOG_LV1("SIG", "insert_new_scan");
	bool inserted = false;
	for (int i = 0; i < SIGNALS_MAX_LEN; i++)
	{
		if (signals[i] == nullptr)
		{
			signals[i] = new Signal(s);
			inserted = true;
			break;
		}
	}
	if (!inserted)
	{
		LOG_LV1("Scan", "Scan array full, scan not inserted.");
	}
}

void decay_signals()
{
	LOG_LV1("SIG", "decay_signals");
	for (int i = 0; i < SIGNALS_MAX_LEN; i++)
	{
		Signal *s = signals[i];
		if (s == nullptr)
		{
			continue;
		}

		if (s->_nextStrength > s->_strength)
		{
			LOG_LV1("SIG", "decay_signals : New strength was equal %f -> %f", s->_strength, s->_nextStrength);
		}
		if (s->_nextStrength > s->_strength)
		{
			LOG_LV1("SIG", "decay_signals : New strength was greater %f -> %f", s->_strength, s->_nextStrength);
			s->_strength = s->_nextStrength;
		}
		else
		{
			float_t decay = min(1.f, max(0.05, s->_scan.decayRate));
			LOG_LV1("SIG", "decay_signals : Decaying lerp( %f, %f %f ) -> %f", s->_strength, s->_nextStrength, decay, lerp(s->_strength, s->_nextStrength, decay));
			s->_strength = lerp(s->_strength, s->_nextStrength, decay);
		}

		// If the signal is too weak, cull it.
		if (s->_strength <= 10)
		{
			LOG_LV1("SIG", "decay_signals :Culling strength less than 1");
			delete s;
			signals[i] = nullptr;
		}
		else
		{
			s->_nextStrength = 0;
		}
	}
}

void signal_loop(int step)
{
	if (mode == AMULET_MODE_AMULET || mode == AMULET_MODE_POWER_AMULET)
	{
		EVERY_N_SECONDS(1)
		decay_signals();
	}
}

Signal *current_top_signal()
{
	float current_strength = 0;
	int current_index = -1;
	int dbg_signal_count = 0;
	for (int i = 0; i < SIGNALS_MAX_LEN; i++)
	{
		Signal *s = signals[i];
		if (s == nullptr)
		{
			continue;
		}
		dbg_signal_count++;
		if (s->_strength > current_strength)
		{
			current_strength = s->_strength;
			current_index = i;
		}
	}

	if (current_index >= 0 && current_index < SIGNALS_MAX_LEN)
	{
		LOG_LV1("SIG", "Current (%d) top signal strength %f. Num signals %d", current_index, signals[current_index]->_strength, dbg_signal_count);
		return signals[current_index];
	}
	else
	{
		LOG_LV1("SIG", "No current top signal. Num signals %d", dbg_signal_count);
		return nullptr;
	}
}
