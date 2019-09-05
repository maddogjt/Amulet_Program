#include "advertising.h"
#include "parameters.h"
#include "packet_types.h"
#include "uart.h"

#include <bluefruit.h>

const char *get_advertisement_type_name(AdvertisementType adType)
{
	switch (adType)
	{
	case AdvertisementType::Amulet:
		return "Amulet";
		break;
	case AdvertisementType::Beacon:
		return "Beacon";
		break;
	case AdvertisementType::Runic:
		return "Rune";
		break;
	case AdvertisementType::Unknown:
	default:
		return "Unknown";
		break;
	}
	return "err";
}

static void debug_print_amulet_mfd(const amulet_mfg_data_t &mfd)
{
	Serial.println("-- Amulet Manufacturer Data --");
	Serial.printf("signal_type: %d\n", mfd.signal_type);
	Serial.printf("power: %d\n", mfd.power);
	Serial.printf("range: %d\n", mfd.range);
	Serial.printf("decay rate: %d\n", mfd.decayRateInt);
	Serial.printf("version:    	%d\n", mfd.version);
	Serial.printBuffer(mfd.payload, kMaxPayloadLen, '-');
	Serial.println();
}

static void start_advertising_uart()
{
	// Advertising packet
	Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
	Bluefruit.Advertising.addTxPower();

	// Include the BLE UART (AKA 'NUS') 128-bit UUID
	Bluefruit.Advertising.addService(uart_get_service());

	// Secondary Scan Response packet (optional)
	// Since there is no room for 'Name' in Advertising packet
	Bluefruit.ScanResponse.addName();

	/* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html   
   */
	Bluefruit.Advertising.restartOnDisconnect(true);
	Bluefruit.Advertising.setInterval(32, 244); // in unit of 0.625 ms
	Bluefruit.Advertising.setFastTimeout(30);   // number of seconds in fast mode
	Bluefruit.Advertising.start(0);				// 0 = Don't stop advertising after n seconds
}

static void start_advertising_with_data(amulet_mfg_data_t &data)
{
	LOG_LV1("BLE", "start_advertising_with_data(), len %d", sizeof(data));
#if CFG_DEBUG >= 1
	debug_print_amulet_mfd(data);
#endif
	// Advertise as non-connectable
	Bluefruit.Advertising.setType(BLE_GAP_ADV_TYPE_NONCONNECTABLE_SCANNABLE_UNDIRECTED);

	// Unknown if this is the best option
	Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);

	// add the amulet's data to the advertisement
	// VERIFY_STATIC(sizeof(data) == 8 + kMaxPayloadLen);
	Bluefruit.Advertising.addManufacturerData(&data, sizeof(data));
	Bluefruit.Advertising.restartOnDisconnect(true);
	Bluefruit.Advertising.setInterval(32, 244); // in unit of 0.625 ms
	Bluefruit.Advertising.setFastTimeout(5);	// number of seconds in fast mode
	Bluefruit.Advertising.start(0);				// 0 = Don't stop advertising after n seconds
}

void advertising_setup(bool advertise, bool uart)
{\
	if (advertise && uart)
	{
		LOG_LV1("BLE", "Error: we can't advertise and have a uart service");
	}

	if (uart)
	{
		Bluefruit.setName("Amulet");
		uart_setup();
		start_advertising_uart();
	}
}

void advertising_start(const AdvertisementType type, const advertisementParams &params, const uint8_t *data, const uint8_t len)
{
	LOG_LV2("BLE", "advertising_start");
	if (len > kMaxPayloadLen)
	{
		LOG_LV1("BLE", "Error: advertisement len (%d) longer than max allowed", len);
		return;
	}

	amulet_mfg_data_t mfdata{};
	mfdata.company_id = kAmuletManufacturerId;
	mfdata.signal_type = (uint8_t)type;
	mfdata.version = kAmuletDataVersion;
	mfdata.power = params.power;
	mfdata.range = params.range;
	mfdata.decayRateInt = params.decay;
	memset(mfdata.payload, 0, kMaxPayloadLen);
	memcpy(&(mfdata.payload), data, len);
	start_advertising_with_data(mfdata);
}

void advertising_stop()
{
	Bluefruit.Advertising.stop();
	Bluefruit.Advertising.clearData();
	Bluefruit.ScanResponse.clearData();
}