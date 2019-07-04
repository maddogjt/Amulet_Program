#include "ble.h"

#include <bluefruit.h>
#define ARDUINO_GENERIC
#include <FastLED.h>
#undef ARDUINO_GENERIC

#include "led.h"
#include "signal.h"

// You are supposed to get manufacturer ids from the bluetooth consortium but I made one up
const uint16_t BLE_AMULET_MFID = 0x69FF;

#define BLE_AMULET_DATA_VERSION (1)

int g_rssi = 0;

typedef struct ATTR_PACKED
{
	uint16_t company_id;
	uint8_t signal_type;
	uint8_t power;
	int8_t range;
	uint8_t decayRateInt;
	uint8_t version;
	uint8_t data[MAX_MFD_DATA_LEN];
} amulet_mfd_t;

void debug_print_amulet_mfd(const amulet_mfd_t &mfd)
{
	Serial.println("-- Amulet Manufacturer Data --");
	Serial.printf("company_id 	%02x\n", mfd.company_id);
	Serial.printf("signal_type: %d\n", mfd.signal_type);
	Serial.printf("power: %d\n", mfd.power);
	Serial.printf("range: %d\n", mfd.range);
	Serial.printf("decay rate: %d\n", mfd.decayRateInt);
	Serial.printf("version:    	%d\n", mfd.version);
	Serial.printBuffer(mfd.data, MAX_MFD_DATA_LEN, '-');
	Serial.println();
}

void scan_callback(ble_gap_evt_adv_report_t *report);
void start_advertising_with_data(amulet_mfd_t &data);

// Uart over BLE service
BLEUart bleuart;
void start_advertising_uart();
void prph_bleuart_rx_callback(uint16_t conn_handle);

void ble_setup(bool advertise, bool scan)
{
	ble_setup(advertise, scan, false);
}

void ble_setup(bool advertise, bool scan, bool uart = false)
{
	if (advertise && uart)
	{
		LOG_LV1("BLE", "Error: we can't advertise and have a uart service");
	}
	// Initialize Bluefruit
	Bluefruit.autoConnLed(false); // Don't blink the led
	if (!Bluefruit.begin(advertise || uart, scan))
	{
		// We need to start as both a peripheral and a central
		digitalWrite(LED_BUILTIN, !LED_STATE_ON);
		Serial.println("Unable to init bluetooth");
	}
	Bluefruit.setTxPower(4); // 4 is (probably) the strongest tx power that we can support.

	if (uart)
	{
		Bluefruit.setName("Amulet");
		// Configure and start the BLE Uart service
		bleuart.begin();
		bleuart.setRxCallback(prph_bleuart_rx_callback);
		start_advertising_uart();
	}

	if (scan)
	{
		Bluefruit.Scanner.setRxCallback(scan_callback); // the callback that lets us process advertising data from other devices
		Bluefruit.Scanner.restartOnDisconnect(true);	// maybe not relevant because we don't plan on connecting to anything
		Bluefruit.Scanner.setInterval(160, 80);			// in unit of 0.625 ms, interval and window (?) perhaps could be tuned
		Bluefruit.Scanner.filterMSD(BLE_AMULET_MFID);   // Only look at advertisements from our made up manufacturer id
		Bluefruit.Scanner.useActiveScan(true);			// I think active scanning allows it to read the manufacturer data
		Bluefruit.Scanner.start(0);						// 0 = Don't stop scanning after n seconds
	}
}

void start_advertising_uart()
{
	// Advertising packet
	Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
	Bluefruit.Advertising.addTxPower();

	// Include the BLE UART (AKA 'NUS') 128-bit UUID
	Bluefruit.Advertising.addService(bleuart);

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

uint8_t speedForButton(bool up)
{
	static uint8_t speeds[] = {3, 7, 11, 14, 20, 44, 78, 120, 200};
	static uint8_t speed_idx = 5;
	speed_idx = min(8, max(0, speed_idx + (up ? 1 : -1)));
	return speeds[speed_idx];
}

Anim animForButton(bool right)
{
	static uint8_t anim_idx = 0;
	anim_idx = (anim_idx + 12 + (right ? 1 : -1)) % 12;
	return (Anim)anim_idx;
}

void prph_bleuart_rx_callback(uint16_t conn_handle)
{
	(void)conn_handle;

	// Wait for new data to arrive
	// uint8_t len = readPacket(&bleuart, 500);
	char str[20 + 1] = {0};
	uint8_t len = bleuart.read(str, 20);

	Serial.print("[Prph] RX: ");
	Serial.println(str);

	if (len == 0 || str[0] != '!')
	{
		return;
	}

	static animPattern ambient = {.name = Anim::AnimSinelon, .params = {}};

	// Color
	if (str[1] == 'C')
	{
		uint8_t red = str[2];
		uint8_t green = str[3];
		uint8_t blue = str[4];
		Serial.print("RGB #");
		if (red < 0x10)
			Serial.print("0");
		Serial.print(red, HEX);
		if (green < 0x10)
			Serial.print("0");
		Serial.print(green, HEX);
		if (blue < 0x10)
			Serial.print("0");
		Serial.println(blue, HEX);

		CHSV color = rgb2hsv_approximate(CRGB(red, green, blue));

		ambient.params.color1_ = color.hue;

		// Set the initial ambient animation
	}

	// Button
	if (str[1] == 'B')
	{
		uint8_t button = str[2];
		uint8_t press = str[3];
		uint8_t unknown = str[4];
		if (press != '0')
		{
			// Only do things on button release
			return;
		}
		if (button == '5') // up
		{
			Serial.print("[Prph] Up");
			ambient.params.speed_ = speedForButton(true);
		}
		else if (button == '6') // down
		{
			ambient.params.speed_ = speedForButton(false);
		}
		else if (button == '7') // left
		{
			ambient.name = animForButton(false);
		}
		else if (button == '8') // right
		{
			ambient.name = animForButton(true);
		}
		else if (button == '1') // 1
		{
			ambient.params.flags_ = ANIMATION_FLAG_FOLD;
		}
		else if (button == '2') // 2
		{
			ambient.params.flags_ = ANIMATION_FLAG_MIRROR;
		}
		else if (button == '3') // 3
		{
			ambient.params.flags_ = ANIMATION_FLAG_LOOP;
		}
		else if (button == '4') // 4
		{
			ambient.params.flags_ = 0;
		}

		// Set the initial ambient animation
	}

	led_set_ambient_animation(ambient);
}

void start_advertising_with_data(amulet_mfd_t &data)
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
	// VERIFY_STATIC(sizeof(data) == 8 + MAX_MFD_DATA_LEN);
	Bluefruit.Advertising.addManufacturerData(&data, sizeof(data));
	Bluefruit.Advertising.restartOnDisconnect(true);
	Bluefruit.Advertising.setInterval(32, 244); // in unit of 0.625 ms
	Bluefruit.Advertising.setFastTimeout(5);	// number of seconds in fast mode
	Bluefruit.Advertising.start(0);				// 0 = Don't stop advertising after n seconds
}

void ble_set_advertisement_data(const AdvertisementType type, const uint8_t *data, const uint8_t len)
{
	LOG_LV2("BLE", "ble_set_advertisement_data");
	if (len > MAX_MFD_DATA_LEN)
	{
		LOG_LV1("BLE", "Error: advertisement len (%d) longer than max allowed", len);
		return;
	}
	amulet_mfd_t mfdata{};
	mfdata.company_id = BLE_AMULET_MFID;
	mfdata.signal_type = (uint8_t)type;
	mfdata.version = BLE_AMULET_DATA_VERSION;
	mfdata.power = 100;
	mfdata.range = -80;
	mfdata.decayRateInt = 0.5 * 255;
	memset(mfdata.data, 0, MAX_MFD_DATA_LEN);
	memcpy(&(mfdata.data), data, len);
	start_advertising_with_data(mfdata);
}

/*------------------------------------------------------------------*/
/* Central (Scanner)
 *------------------------------------------------------------------*/
void scan_callback(ble_gap_evt_adv_report_t *report)
{
	uint8_t len = 0;
	uint8_t buffer[32];
	memset(buffer, 0, sizeof(buffer));

	/* Check for Manufacturer Specific Data */
	len = Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA, buffer, sizeof(buffer));
	amulet_mfd_t mfd;
	if (len == sizeof(amulet_mfd_t))
	{
		memcpy(&mfd, buffer, sizeof(mfd));
		memset(buffer, 0, sizeof(buffer));

#if CFG_DEBUG >= 2
		debug_print_amulet_mfd(mfd);
#endif

		bool signalIsValid = true;

		if (BLE_AMULET_DATA_VERSION != mfd.version)
		{
			LOG_LV2("Scan", "Invalid signal. Scan version (%d) is not equal to our version (%d)", mfd.version, BLE_AMULET_DATA_VERSION);
			signalIsValid = false;
		}

		// Only report signals that are in range
		if (report->rssi < mfd.range)
		{
			LOG_LV2("Scan", "Invalid signal. RSSI (%d) is weaker than range (%d)", report->rssi, mfd.range);
			signalIsValid = false;
		}

		const int8_t AMBIENT_CUTOFF = 10; // Need to put this in a better place.
		if (mfd.power < AMBIENT_CUTOFF)
		{
			LOG_LV2("Scan", "Invalid signal. Power (%d) is weaker than cutoff (%d)", mfd.power, AMBIENT_CUTOFF);
			signalIsValid = false;
		}

		if (signalIsValid)
		{
			// set the signal strength
			LOG_LV2("Scan", "%14s %d dBm\n", "RSSI", report->rssi);
			g_rssi = report->rssi;

			Scan s = {
				.rssi = report->rssi,
				.signal_type = mfd.signal_type,
				.power = mfd.power,
				.decayRate = ((float)mfd.decayRateInt / 255.f),
			};
			s.setData(mfd.data);
			// Save this entry to signals
			add_scan_data(s);
		}

		Bluefruit.Scanner.resume();
	}
	else
	{
		LOG_LV1("Scan", "Incorrect size of manufacturers data. Expected %d", sizeof(amulet_mfd_t));
		LOG_LV1_BUFFER("Scan", buffer, len);
		Bluefruit.Scanner.resume();
	}
}
