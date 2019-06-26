#include "ble.h"

#include <bluefruit.h>

#include "signal.h"

// You are supposed to get manufacturer ids from the bluetooth consortium but I made one up
const uint16_t BLEAMULET_MFID = 0xFF22; 

int g_rssi =0;

amulet_mfd_t ad_data;
amulet_mfd_t viral_ad_data;
uint32_t viral_ad_timestamp = 0;
bool broadcasting_viral_data = false;

void scan_callback(ble_gap_evt_adv_report_t* report);
void start_advertising_with_data(amulet_mfd_t &data);

void ble_setup()
{
  // Initialize Bluefruit
  Bluefruit.autoConnLed(false); // Don't blink the led
  if( !Bluefruit.begin(1,1) ) {  // We need to start as both a peripheral and a central    
      digitalWrite(LED_BUILTIN, !LED_STATE_ON);
      Serial.println("Unable to init bluetooth");
  }         
  Bluefruit.setTxPower(4);      // 4 is (probably) the strongest tx power that we can support.

  Bluefruit.Scanner.setRxCallback(scan_callback); // the callback that lets us process advertising data from other devices
  Bluefruit.Scanner.restartOnDisconnect(true);  // maybe not relevant because we don't plan on connecting to anything
  Bluefruit.Scanner.setInterval(160, 80);       // in unit of 0.625 ms, interval and window (?) perhaps could be tuned
  Bluefruit.Scanner.filterMSD(BLEAMULET_MFID);  // Only look at advertisements from our made up manufacturer id
  Bluefruit.Scanner.useActiveScan(true);        // I think active scanning allows it to read the manufacturer data
  Bluefruit.Scanner.start(0);                   // 0 = Don't stop scanning after n seconds

  // Set up the amulet's custom payload 
  ad_data =
  {
      .company_id         = BLEAMULET_MFID,
      .command            = (int)command_beacon,
      .param0             = 0x02,
      .param1             = 0x01,
      .param2             = 0x01,
  };
  start_advertising_with_data(ad_data);
}

void start_advertising_with_data(amulet_mfd_t &data)
{
#if CFG_DEBUG >= 1
  Serial.println("--START ADVERTISING WITH DATA--");
  Serial.printf("company_id %02x\n",data.company_id);
  Serial.printf("command:   %d\n",data.command);
  Serial.printf("param0:    %d\n",data.param0);
  Serial.printf("param1:    %d\n",data.param1);
  Serial.printf("param2:    %d\n",data.param2);

  // Serial.printf("%14s ", "MAN SPEC DATA");
  // Serial.printBuffer(buffer, len, '-');
Serial.println();
#endif

  // Advertise as non-connectable
  Bluefruit.Advertising.setType(BLE_GAP_ADV_TYPE_NONCONNECTABLE_SCANNABLE_UNDIRECTED);

  // Unknown if this is the best option
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);

  // add the amulet's data to the advertisement
  VERIFY_STATIC( sizeof(data) == 6);
  Bluefruit.Advertising.addManufacturerData(&data, sizeof(data));
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(5);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds
}

void ble_set_advertisement_data( amulet_mfd_t &data)
{
  LOG_LV1("BLE","ble_set_advertisement_data");
  ad_data = data;
  broadcasting_viral_data = false;
  start_advertising_with_data(ad_data);
}

void ble_viral_override( amulet_mfd_t &data)
{
  LOG_LV1("BLE","ble_viral_override");
  // If we aren't broadcasting viral data,
  // and its been over 30 seconds since our last viral broadcast, 
  // set the viral payload.
  if( !broadcasting_viral_data && millis() - viral_ad_timestamp > 30*1000) {
    broadcasting_viral_data = true;
    viral_ad_data = data;
    viral_ad_timestamp = millis();
    start_advertising_with_data(ad_data);
  }
}

void resume_normal_ad_data() {
  // if we've been broadcasting viral data for more than 1s switch back to normal data
  if( broadcasting_viral_data && millis() - viral_ad_timestamp > 1000 ) {
    LOG_LV1("BLE","resume_normal_ad_data - resuming!");
    ble_set_advertisement_data(ad_data);
  }
}

void ble_loop( int step)
{
  if( step %24 == 0 ) { 
    // only run the viral pattern for a second
    resume_normal_ad_data();  
  } 
}

/*------------------------------------------------------------------*/
/* Central (Scanner)
 *------------------------------------------------------------------*/
void scan_callback(ble_gap_evt_adv_report_t* report)
{
    uint8_t len = 0;
    uint8_t buffer[32];
    memset(buffer, 0, sizeof(buffer));


      /* Check for Manufacturer Specific Data */
    len = Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA, buffer, sizeof(buffer));
    amulet_mfd_t data;
    if (len == sizeof(amulet_mfd_t)) // possible enhancement: allow the length to be shorter to accomodate future revisions with more fields
    {
        memcpy(&data,buffer,sizeof(data));
        memset(buffer, 0, sizeof(buffer));

#if CFG_DEBUG >= 2
        Serial.printf("company_id %02x\n",data.company_id);
        Serial.printf("command:   %d\n",data.command);
        Serial.printf("param0:    %d\n",data.param0);
        Serial.printf("param1:    %d\n",data.param1);
        Serial.printf("param2:    %d\n",data.param2);

        // Serial.printf("%14s ", "MAN SPEC DATA");
        // Serial.printBuffer(buffer, len, '-');
        Serial.println();
#endif

        // set the signal strength
        LOG_LV2("Scan","%14s %d dBm\n", "RSSI", report->rssi);
        g_rssi = report->rssi;

        Scan s = {
          ._rssi = report->rssi,
          ._cmd = (amulet_command_t) data.command,
          ._p0 = data.param0,
          ._p1 = data.param1,
          ._p2 = data.param2,
          ._timestamp = millis(),
        };
        // Save this entry to signals
        add_scan_data( s );

        Bluefruit.Scanner.resume();

    } else {
      LOG_LV1("Scan","Incorrect size of manufacturers data. Expected %d",sizeof(amulet_mfd_t));
      LOG_LV1_BUFFER("Scan", buffer, len);
      Bluefruit.Scanner.resume();
    }
}
