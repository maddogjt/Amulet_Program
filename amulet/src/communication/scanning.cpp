#include "scanning.h"
#include "packet_types.h"
#include "parameters.h"

#include <bluefruit.h>

#include "../../settings.h"

static void scan_callback(ble_gap_evt_adv_report_t *report);

void scanning_setup()
{
	Bluefruit.Scanner.setRxCallback(scan_callback); // the callback that lets us process advertising data from other devices
	Bluefruit.Scanner.restartOnDisconnect(false);	// maybe not relevant because we don't plan on connecting to anything
	Bluefruit.Scanner.setInterval(160, 80);			// in unit of 0.625 ms, interval and window (?) perhaps could be tuned
	Bluefruit.Scanner.filterMSD(kAmuletManufacturerId); // Only look at advertisements from our made up manufacturer id
	Bluefruit.Scanner.useActiveScan(true);			// I think active scanning allows it to read the manufacturer data
	Bluefruit.Scanner.start(0);						// 0 = Don't stop scanning after n seconds
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
	amulet_mfg_data_t mfd;
	if (len == sizeof(amulet_mfg_data_t))
	{
		memcpy(&mfd, buffer, sizeof(mfd));
		memset(buffer, 0, sizeof(buffer));

#if CFG_DEBUG >= 2
		debug_print_amulet_mfd(mfd);
#endif

		bool signalIsValid = true;

		if (kAmuletDataVersion != mfd.version)
		{
			LOG_LV2("Scan", "Invalid signal. Scan version (%d) is not equal to our version (%d)", mfd.version, kAmuletDataVersion);
			signalIsValid = false;
		}

		// Only report signals that are in range
		if (report->rssi < mfd.range)
		{
			LOG_LV2("Scan", "Invalid signal. RSSI (%d) is weaker than range (%d)", report->rssi, mfd.range);
			signalIsValid = false;
		}

		if (mfd.power < globalSettings_.ambientPowerThreshold_)
		{
			LOG_LV2("Scan", "Invalid signal. Power (%d) is weaker than cutoff (%d)", mfd.power, globalSettings_.ambientPowerThreshold_);
			signalIsValid = false;
		}

		if (signalIsValid)
		{
			// set the signal strength
			LOG_LV2("Scan", "%14s %d dBm\n", "RSSI", report->rssi);

			Scan s = {
				.rssi = report->rssi,
				.signal_type = mfd.signal_type,
				.power = mfd.power,
				.decayRate = ((float)mfd.decayRateInt / 255.f),
			};
			s.setData(mfd.payload);
			// Save this entry to signals
			add_scan_data(s);
		}
	}
	else
	{
		LOG_LV1("Scan", "Incorrect size of manufacturers data. Expected %d", sizeof(amulet_mfg_data_t));
		LOG_LV1_BUFFER("Scan", buffer, len);
	}
		
	Bluefruit.Scanner.resume();
}
