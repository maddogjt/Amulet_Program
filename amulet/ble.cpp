#include "ble.h"

#include <bluefruit.h>
#define ARDUINO_GENERIC
#include <FastLED.h>
#undef ARDUINO_GENERIC

#include "led.h"
#include "signal.h"
#include "settings.h"
#include "Startup.h"

// You are supposed to get manufacturer ids from the bluetooth consortium but I made one up
const uint16_t BLE_AMULET_MFID = 0x69FF;

#define BLE_AMULET_DATA_VERSION (1)

int g_rssi = 0;
StartupConfig config = localSettings_.startupConfig_;
animPattern ambient = {.name = Anim::AnimSinelon, .params = {}};

typedef struct ATTR_PACKED
{
	uint16_t company_id;
	uint8_t signal_type;
	uint8_t power;
	int8_t range;
	uint8_t decayRateInt; // essentially a fract8
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

void ble_setup(bool advertise, bool scan, bool uart)
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
	Bluefruit.setTxPower(globalSettings_.txPower_); // 4 is (probably) the strongest tx power that we can support.

	if (uart)
	{
		Bluefruit.setName("Amulet");

		// populate with ambient
		ambient = led_get_ambient_animation();
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

void colorCycle(bool next, uint8_t idx)
{
	uint8_t hue = (idx == 1) ? ambient.params.color1_ : ambient.params.color2_;
	hue = (hue + 255 + (next ? 16 : -16)) % 255;
	bleuart.printf("P: color%d V: %d\n", idx + 1, hue);

	if (idx == 1)
	{
		ambient.params.color1_ = hue;
	}
	else
	{
		ambient.params.color2_ = hue;
	}
}
void extraCycle(bool next, uint8_t idx)
{
	uint8_t &extra = (idx == 7) ? ambient.params.extra0_ : ambient.params.extra1_;
	extra = (extra + 255 + (next ? 16 : -16)) % 255;
	bleuart.printf("P: extra[%d] V: %d\n", (idx == 7) ? 0 : 1, extra);
}

void speedCycle(bool next, uint8_t unused)
{
	uint8_t speed = ambient.params.speed_;
	speed = (speed + 255 + (next ? 16 : -16)) % 255;
	bleuart.printf("P: speed V: %d\n", speed);
	ambient.params.speed_ = speed;
}

void flagCycle(bool next, uint8_t idx)
{
	uint8_t flags_idx = 0;
	uint8_t flags[] = {0, ANIMATION_FLAG_FOLD, ANIMATION_FLAG_MIRROR, ANIMATION_FLAG_LOOP};
	const char *flag_names[] = {"None", "Fold", "Mirror", "Loop"};
	for (int i = 0; i < 4; i++)
	{
		if (flags[i] == ambient.params.flags_)
		{
			flags_idx = i;
			break;
		}
	}
	flags_idx = (flags_idx + 4 + (next ? 1 : -1)) % 4;
	bleuart.printf("P: flag V: %s\n", flag_names[flags_idx]);
	ambient.params.flags_ = flags[flags_idx];
}

void modCycle(bool next, uint8_t idx)
{
	uint8_t mod_alpha = ambient.params.mods_ & 0x0F;
	uint8_t mod_blend = ambient.params.mods_ & 0xF0;
	mod_blend = mod_blend >> 4;
	if (idx == 5)
	{
		mod_alpha = (mod_alpha + 16 + (next ? 1 : -1)) % 16;
		bleuart.printf("P: mask V: %d\n", mod_alpha);
	}
	else
	{
		mod_blend = (mod_blend + 16 + (next ? 1 : -1)) % 16;
		bleuart.printf("P: mod V: %d\n", mod_blend);
	}
	mod_blend = mod_blend << 4;
	bleuart.printf("P: mod V: %d\n", mod_alpha | mod_blend);
	ambient.params.mods_ = mod_alpha | mod_blend;
}

void animCycle(bool next, uint8_t unused)
{
	uint8_t anim_idx = (uint8_t)ambient.name;
	anim_idx = (anim_idx + get_animations_count() + (next ? 1 : -1)) % get_animations_count();
	bleuart.printf("P:anim V:%.10s\n", get_animation_name((Anim)anim_idx));
	ambient.name = (Anim)anim_idx;
}

void modeCycle(bool next, uint8_t unused)
{
	uint8_t mode_idx = 0;
	const amulet_mode_t modes[] = {AMULET_MODE_AMULET, AMULET_MODE_BEACON, AMULET_MODE_RUNE, AMULET_MODE_BEACON_POWER_AMULET};
	for (int i = 0; i < 4; i++)
	{
		if (modes[i] == config.mode)
		{
			mode_idx = i;
			break;
		}
	}
	mode_idx = (mode_idx + 4 + (next ? 1 : -1)) % 4;
	bleuart.printf("P: mode V: %s\n", get_config_mode_name(modes[mode_idx]));
	config.mode = modes[mode_idx];
}

void powerCycle(bool next, uint8_t unused)
{
	uint8_t power = config.ad.power;
	power = (power + 255 + (next ? 16 : -16)) % 255;
	bleuart.printf("P: power V: %d\n", power);
	config.ad.power = power;
}

void decayCycle(bool next, uint8_t unused)
{
	uint8_t decay = config.ad.decay;
	decay = (decay + 255 + (next ? 16 : -16)) % 255;
	bleuart.printf("P: decay V: %4.2f%%\n", (float)decay * 100.f / 255.f);
	config.ad.decay = decay;
}

void rangeCycle(bool next, uint8_t unused)
{
	int8_t range = config.ad.range;
	range = max(-120, min(0, range + (next ? 2 : -2)));
	bleuart.printf("P: range V: %d\n", range);
	config.ad.range = range;
}

typedef void (*ParameterCycleList[])(bool next, uint8_t idx);
ParameterCycleList g_ConfigCyclers = {
	modeCycle,
	powerCycle,
	decayCycle,
	rangeCycle,
};
const char *g_ConfigCyclerNames[] = {
	"mode",
	"power",
	"decay",
	"range",
};

ParameterCycleList g_AnimCyclers = {
	animCycle,
	colorCycle,
	colorCycle,
	speedCycle,
	flagCycle,
	modCycle,
	modCycle,
	extraCycle,
	extraCycle,
};
const char *g_AnimCyclerNames[] = {
	"animation",
	"color 1",
	"color 2",
	"speed",
	"flag",
	"mods alpha",
	"mods blend",
	"extra 1",
	"extra 2",
};
uint8_t g_AnimIdx = 0;
uint8_t g_ConfigIdx = 0;
bool g_tweaker_is_modifying_animation = true;

bool g_ble_uart_packet_in_progress = false;

void prph_bleuart_rx_callback(uint16_t conn_handle)
{
	(void)conn_handle;

	// Wait for new data to arrive
#define MAX_BLE_UART_PACKET 200
	static char str[MAX_BLE_UART_PACKET] = {0};
	static uint8_t len = 0;

	int avail = bleuart.available();
	if (avail <= 0)
	{
		return;
	}

	if (avail + len > MAX_BLE_UART_PACKET)
	{
		Serial.println("ERROR: Buffer Overflow");
	}

	if (g_ble_uart_packet_in_progress)
	{
		if (bleuart.peek() == '!')
		{
			LOG_LV1("BLE", "Old packet incomplete, starting new one.");
			memset(str, 0, MAX_BLE_UART_PACKET);
			len = bleuart.read(str, avail);
		}
		else
		{
			len += bleuart.read(str + strlen(str), avail);
		}
	}
	else
	{
		memset(str, 0, MAX_BLE_UART_PACKET);
		len = bleuart.read(str, avail);
	}

	Serial.print("[Prph] RX: ");
	Serial.println(str);

	if (str[0] != '!')
	{
		Serial.println("BLEUart service got rogue packet");
		Serial.printBuffer(str, len);
		return;
	}

	// StartupConfig
	if (str[1] == 'S')
	{
		if (str[strlen(str) - 1] != '#')
		{
			g_ble_uart_packet_in_progress = true;
			return;
		}
		g_ble_uart_packet_in_progress = false;
		char *configStr = &str[2];

		StartupConfig config = deserializeStartupConfig(configStr, len - 2);
		Serial.println("Setting startup config from BLE service");
		Serial.printf("%s", configStr);
		localSettings_.startupConfig_ = config;
		write_local_settings();
		startWithConfig(config);
		return;
	}

	// Set Ambient Animatoin
	if (str[1] == 'A')
	{
		if (str[strlen(str) - 1] != '#')
		{
			g_ble_uart_packet_in_progress = true;
			return;
		}
		g_ble_uart_packet_in_progress = false;

		char *animStr = &str[2];
		animPattern pattern = deserializeAnimPattern(animStr, len - 2);
		Serial.println("Setting animation pattern from BLE service");
		Serial.printf("%s", animStr);
		localSettings_.startupConfig_.pattern = pattern;
		//write_local_settings();
		led_set_ambient_animation(pattern);
		return;
	}

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

		// Set the ambient animation
		led_set_ambient_animation(ambient);
		return;
	}

	// Button
	if (str[1] == 'B')
	{
		uint8_t button = str[2];
		uint8_t press = str[3];
		// uint8_t unknown = str[4];
		if (press != '0')
		{
			// Only do things on button release
			return;
		}
		if (button == '5') // up
		{
			if (g_tweaker_is_modifying_animation)
			{
				g_AnimIdx = (g_AnimIdx + 8) % 9;
				bleuart.printf("P: %s\n", g_AnimCyclerNames[g_AnimIdx]);
			}
			else
			{
				g_ConfigIdx = (g_ConfigIdx + 3) % 4;
				bleuart.printf("P: %s\n", g_ConfigCyclerNames[g_ConfigIdx]);
			}
		}
		else if (button == '6') // down
		{
			if (g_tweaker_is_modifying_animation)
			{
				g_AnimIdx = (g_AnimIdx + 1) % 9;
				bleuart.printf("P: %s\n", g_AnimCyclerNames[g_AnimIdx]);
			}
			else
			{
				g_ConfigIdx = (g_ConfigIdx + 1) % 4;
				bleuart.printf("P: %s\n", g_ConfigCyclerNames[g_ConfigIdx]);
			}
		}
		else if (button == '7' || button == '8') // left or right
		{
			if (g_tweaker_is_modifying_animation)
			{
				g_AnimCyclers[g_AnimIdx](button == '8', g_AnimIdx);
			}
			else
			{
				g_ConfigCyclers[g_ConfigIdx](button == '8', g_ConfigIdx);
			}
		}
		else if (button == '1') // The Info Button
		{
			if (g_tweaker_is_modifying_animation)
			{
				bleuart.printf("Anim: %s\n", get_animation_name(ambient.name));
			}
			else
			{
				bleuart.printf("Conf: %s\n", get_config_mode_name(config.mode));
			}
		}
		else if (button == '2') // The Info Details Button
		{
			if (g_tweaker_is_modifying_animation)
			{
				bleuart.printf("mask: %d\n", ambient.params.mods_ & 0x0F);
			}
			else
			{
				bleuart.printf(":P\n");
			}
		}
		else if (button == '3') // The mode switcher button
		{
			if (g_tweaker_is_modifying_animation)
			{
				bleuart.printf("Editing config\n");
				g_tweaker_is_modifying_animation = false;
			}
			else
			{
				bleuart.printf("Editing anim\n");
				g_tweaker_is_modifying_animation = true;
			}
		}
		else if (button == '4') // Commit the changes button (after system reset)
		{
			bleuart.printf("Saving Config\n");

			config.pattern = ambient;
			localSettings_.startupConfig_ = config;
			write_local_settings();
		}

		// Set the ambient animation
		led_set_ambient_animation(ambient);
		return;
	}

	Serial.print("[Prph] Unknown BLE Uart command RX: ");
	Serial.println(str);
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

void ble_set_advertisement_data(const AdvertisementType type, const advertisementParams &params, const uint8_t *data, const uint8_t len)
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
	mfdata.power = params.power;
	mfdata.range = params.range;
	mfdata.decayRateInt = params.decay;
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
