
#include "signal.h"

#include "../settings/settings.h"

static Signal *gTopSignal = nullptr;

// Precise method, which guarantees v = v1 when t = 1.
float lerp(float v0, float v1, float t)
{
	return (1 - t) * v0 + t * v1;
}

float compute_strength(int8_t rssi, const amulet_mfg_data_t &data) {
	return data.power;
}

void signal_add_scan_data(int8_t rssi, const amulet_mfg_data_t &data)
{
	// if (gMode == AMULET_MODE_RUNE && s.signal_type != (uint8_t)AdvertisementType::Amulet)
	// {
	// 	// Runes only care about amulets;
	// 	return;
	// }

	if (data.signal_type == (uint8_t)AdvertisementType::Amulet)
	{
		// Amulets do not care about amulets;
		return;
	}

	float strength = compute_strength(rssi, data);

	if (gTopSignal = nullptr) {
		gTopSignal = new Signal(rssi, data, strength);
		return;
	}


	if (0 == memcmp(&gTopSignal->data_, &data, sizeof(data)))
	{
		// Update the top signal
		gTopSignal->signalStrength_ = max(gTopSignal->signalStrength_, strength);
		gTopSignal->seenCount_++;
		gTopSignal->rssi_ = rssi;
		return;
	}

	if (strength > gTopSignal->strength_)
	{
		// New top signal
		if (gTopSignal != nullptr)
		{
			delete gTopSignal;
			gTopSignal = nullptr;
		}
		gTopSignal = new Signal(rssi, data, strength);
		gTopSignal->signalStrength_ = strength;
		LOG_LV1("SIGNAL", "New top signal: %s", get_advertisement_type_name((AdvertisementType)gTopSignal->data_.signal_type));
		LOG_LV1("SIGNAL", "    Power: %d, RSSI: %d", gTopSignal->data_.power, gTopSignal->rssi_);
	}
}

void decay_signals()
{
	if (gTopSignal == nullptr)
	{
		return;
	}

	if (gTopSignal->signalStrength_ >= gTopSignal->strength_)
	{
		// LOG_LV3("SIG", "decay_signals : New strength was greater %f -> %f", topSignal->_strength, topSignal->_nextStrength);
		gTopSignal->strength_ = gTopSignal->signalStrength_;
	}
	else
	{
		float decayRate = gTopSignal->data_.decayRateInt/255.f;
		float_t decay = min(1.f, max(0.05f, decayRate));
		// LOG_LV3("SIG", "decay_signals : Decaying lerp( %f, %f %f ) -> %f", topSignal->_strength, topSignal->_nextStrength, decay, lerp(topSignal->_strength, topSignal->_nextStrength, decay));
		gTopSignal->strength_ = lerp(gTopSignal->strength_, gTopSignal->signalStrength_, decay);
	}

	// If the signal is too weak, cull it.
	if (gTopSignal->strength_ <= globalSettings_.ambientPowerThreshold_)
	{
		// LOG_LV3("SIG", "decay_signals :Culling strength less than %d", globalSettings_.ambientPowerThreshold_);
		delete gTopSignal;
		gTopSignal = nullptr;
		LOG_LV1("SIGNAL", "New top signal: NULL (decay)");
	}
	else
	{
		gTopSignal->signalStrength_ = 0;
	}
}

void signal_loop(int step)
{
	EVERY_N_SECONDS(1)
	{
		decay_signals();
	}
}

Signal *signal_get_current_top()
{
	return gTopSignal;
}
