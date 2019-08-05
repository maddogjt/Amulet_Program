#pragma once

static constexpr size_t kMaxPayloadLen = 14;

struct ATTR_PACKED amulet_mfg_data_t
{
	//uint16_t company_id;
	uint8_t signal_type;
	uint8_t power;
	int8_t range;
	uint8_t decayRateInt; // essentially a fract8
	uint8_t version;
	uint8_t payload[kMaxPayloadLen];
};
