#define ARDUINO_GENERIC
#include <FastLED.h>
#undef ARDUINO_GENERIC
#include <bluefruit.h>

#include <JC_Button.h>

FASTLED_USING_NAMESPACE

Button dfuButton(PIN_DFU, 25, true, true);

constexpr int singleClick = 1;
constexpr int doubleClick = 2;
constexpr int longHold = 3;

// FastLED "100-lines-of-code" demo reel, showing just a few
// of the kinds of animation patterns you can quickly and easily
// compose using FastLED.
//
// This example also shows one easy way to define multiple
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN PIN_RGB_LEDS
//#define CLK_PIN   4
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS RGB_LED_COUNT
CRGB leds[NUM_LEDS];

#define BRIGHTNESS 8
#define FRAMES_PER_SECOND 120

  void setup()
  {
    pinMode(28, INPUT_PULLUP);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LED_STATE_ON);
    Serial.begin(115200);
    // while (!Serial)
    //    delay(10); // for nrf52840 with native usb

  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined(__AVR_ATtiny85__)
    if (F_CPU == 16000000)
      clock_prescale_set(clock_div_1);
  #endif
    // End of trinket special codex
     //digitalWrite(LED_BUILTIN, HIGH);

  // Serial.println("Wheee!");

  pinMode(PIN_RGB_LED_PWR, OUTPUT);
  digitalWrite(PIN_RGB_LED_PWR, RGB_LED_PWR_ON);


  dfuButton.begin();

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, RGB_LED_COUNT).setCorrection(TypicalLEDStrip);
  // FastLED.addLeds<LED_TYPE, 16, COLOR_ORDER>(leds + 4, 4).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);


  // // Initialize Bluefruit with maximum connections as Peripheral = 0, Central = 1
  // // SRAM usage required by SoftDevice will increase dramatically with number of connections
  // Bluefruit.begin(0, 1);
  // Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values
  // Bluefruit.setName("Bluefruit52");

  // // Start Central Scan
  // Bluefruit.setConnLedInterval(250);
  // Bluefruit.Scanner.setRxCallback(scan_callback);
  // Bluefruit.Scanner.start(0);

}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = {
    rainbow,
    rainbow2,
    purple,
    cylon,
    //red,
    //green,
    //blue, 
    confetti, sinelon, cycleHSV};

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0;                  // rotating "base color" used by many of the patterns

uint32_t lastMillis = 0;
bool ledState = 0;
void loop()
{

 // Serial.println("Looping!");
  // if ((millis() - lastMillis) > 2000) {
  //   lastMillis = millis();
  //   ledState = !ledState;
  //   digitalWrite(LED_BUILTIN, ledState);
  //  Serial.println("Looping!");
  // }


  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000 / FRAMES_PER_SECOND);

  // do some periodic updates
  EVERY_N_MILLISECONDS(20) { gHue++; }   // slowly cycle the "base color" through the rainbow

  dfuButton.read();

  if (dfuButton.pressedFor(2000)) {
    Serial.println("Going to sleep now");
//    delay(500);

    digitalWrite(PIN_RGB_LED_PWR, !RGB_LED_PWR_ON);
    digitalWrite(LED_BUILTIN, !LED_STATE_ON);

    systemOff(29, 0);

  } else if (dfuButton.wasReleased())
  { // Just a click event to advance to next pattern
    // gCurrentPatternNumber = (gCurrentPatternNumber + 1) % maxMode;
    nextPattern();
    Serial.print("Switching pattern to ");
    Serial.println(gCurrentPatternNumber);

  }


//  EVERY_N_SECONDS(10) { nextPattern(); } // change patterns periodically
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns);
}

void mirror()
{
  for (int i = 0; i < 4; i++) {
    leds[7-i] = leds[i];
  }
}

void red()
{
  fill_solid(leds, NUM_LEDS, CRGB(255, 0, 0));
}

void green()
{
  fill_solid(leds, NUM_LEDS, CRGB(0, 255, 0));
}

void blue()
{
  fill_solid(leds, NUM_LEDS, CRGB(0, 0, 255));
}

void purple()
{
  fill_solid(leds, NUM_LEDS, CRGB(255, 0, 255));
}


void rainbow()
{
  // FastLED's built-in rainbow generator
  fill_rainbow(leds, NUM_LEDS, gHue, 20);
}


void rainbow2()
{
  // FastLED's built-in rainbow generator
  fill_rainbow(leds, 4, gHue, 20);
  mirror();
}

void cylon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(leds, 4, 20);
  int pos = beatsin16(26, 0, 4 - 1);
  leds[pos] += CHSV(gHue, 255, 192);
  mirror();
}

void addGlitter(fract8 chanceOfGlitter)
{
  if (random8() < chanceOfGlitter)
  {
    leds[random16(NUM_LEDS)] += CRGB::White;
  }
}

void rainbowWithGlitter()
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}
void confetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy(leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV(gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS - 1);
  leds[pos] += CHSV(gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
  for (int i = 0; i < NUM_LEDS; i++)
  { //9948
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
}

void juggle()
{
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy(leds, NUM_LEDS, 20);
  byte dothue = 0;
  for (int i = 0; i < 8; i++)
  {
    leds[beatsin16(i + 7, 0, NUM_LEDS - 1)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void cycleHSV()
{
  fill_solid(leds, NUM_LEDS, CHSV(gHue, 255, 192));
}

void scan_callback(ble_gap_evt_adv_report_t *report)
{
    PRINT_LOCATION();
    uint8_t len = 0;
    uint8_t buffer[32];
    memset(buffer, 0, sizeof(buffer));

    /* Display the timestamp and device address */
    if (report->type.scan_response)
    {
        Serial.printf("[SR%10d] Packet received from ", millis());
    }
    else
    {
        Serial.printf("[ADV%9d] Packet received from ", millis());
    }
    // MAC is in little endian --> print reverse
    Serial.printBufferReverse(report->peer_addr.addr, 6, ':');
    Serial.print("\n");

    /* Raw buffer contents */
    Serial.printf("%14s %d bytes\n", "PAYLOAD", report->data.len);
    if (report->data.len)
    {
        Serial.printf("%15s", " ");
        Serial.printBuffer(report->data.p_data, report->data.len, '-');
        Serial.println();
    }

    /* RSSI value */
    Serial.printf("%14s %d dBm\n", "RSSI", report->rssi);

    /* Adv Type */
    Serial.printf("%14s ", "ADV TYPE");
    if (report->type.connectable)
    {
        Serial.print("Connectable ");
    }
    else
    {
        Serial.print("Non-connectable ");
    }

    if (report->type.directed)
    {
        Serial.println("directed");
    }
    else
    {
        Serial.println("undirected");
    }

    /* Shortened Local Name */
    if (Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_SHORT_LOCAL_NAME, buffer, sizeof(buffer)))
    {
        Serial.printf("%14s %s\n", "SHORT NAME", buffer);
        memset(buffer, 0, sizeof(buffer));
    }

    /* Complete Local Name */
    if (Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME, buffer, sizeof(buffer)))
    {
        Serial.printf("%14s %s\n", "COMPLETE NAME", buffer);
        memset(buffer, 0, sizeof(buffer));
    }

    /* TX Power Level */
    if (Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_TX_POWER_LEVEL, buffer, sizeof(buffer)))
    {
        Serial.printf("%14s %i\n", "TX PWR LEVEL", buffer[0]);
        memset(buffer, 0, sizeof(buffer));
    }

    /* Check for UUID16 Complete List */
    len = Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_16BIT_SERVICE_UUID_COMPLETE, buffer, sizeof(buffer));
    if (len)
    {
        printUuid16List(buffer, len);
    }

    /* Check for UUID16 More Available List */
    len = Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_16BIT_SERVICE_UUID_MORE_AVAILABLE, buffer, sizeof(buffer));
    if (len)
    {
        printUuid16List(buffer, len);
    }

    /* Check for UUID128 Complete List */
    len = Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_128BIT_SERVICE_UUID_COMPLETE, buffer, sizeof(buffer));
    if (len)
    {
        printUuid128List(buffer, len);
    }

    /* Check for UUID128 More Available List */
    len = Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_128BIT_SERVICE_UUID_MORE_AVAILABLE, buffer, sizeof(buffer));
    if (len)
    {
        printUuid128List(buffer, len);
    }

    /* Check for BLE UART UUID */
    if (Bluefruit.Scanner.checkReportForUuid(report, BLEUART_UUID_SERVICE))
    {
        Serial.printf("%14s %s\n", "BLE UART", "UUID Found!");
    }

    /* Check for DIS UUID */
    if (Bluefruit.Scanner.checkReportForUuid(report, UUID16_SVC_DEVICE_INFORMATION))
    {
        Serial.printf("%14s %s\n", "DIS", "UUID Found!");
    }

    /* Check for Manufacturer Specific Data */
    len = Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA, buffer, sizeof(buffer));
    if (len)
    {
        Serial.printf("%14s ", "MAN SPEC DATA");
        Serial.printBuffer(buffer, len, '-');
        Serial.println();
        memset(buffer, 0, sizeof(buffer));
    }

    Serial.println();

    // For Softdevice v6: after received a report, scanner will be paused
    // We need to call Scanner resume() to continue scanning
    Bluefruit.Scanner.resume();
}

void printUuid16List(uint8_t *buffer, uint8_t len)
{
    Serial.printf("%14s %s", "16-Bit UUID");
    // for (int i = 0; i < len; i += 2)
    // {
    //     uint16_t uuid16;
    //     memcpy(&uuid16, buffer + i, 2);
    //     Serial.printf("%04X ", uuid16);
    // }
    Serial.println();
}

void printUuid128List(uint8_t *buffer, uint8_t len)
{
    (void)len;
    Serial.printf("%14s %s", "128-Bit UUID");

    // Print reversed order
    for (int i = 0; i < 16; i++)
    {
        const char *fm = (i == 4 || i == 6 || i == 8 || i == 10) ? "-%02X" : "%02X";
        Serial.printf(fm, buffer[15 - i]);
    }

    Serial.println();
}

// void startAdv(void)
// {   
//   // Advertising packet
//   Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
//   Bluefruit.Advertising.addTxPower();
//   Bluefruit.Advertising.addName();

//   /* Start Advertising
//    * - Enable auto advertising if disconnected
//    * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
//    * - Timeout for fast mode is 30 seconds
//    * - Start(timeout) with timeout = 0 will advertise forever (until connected)
//    * 
//    * For recommended advertising interval
//    * https://developer.apple.com/library/content/qa/qa1931/_index.html
//    */
//   Bluefruit.Advertising.setStopCallback(adv_stop_callback);
//   Bluefruit.Advertising.restartOnDisconnect(true);
//   Bluefruit.Advertising.setInterval(32, 244);    // in units of 0.625 ms
//   Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
//   Bluefruit.Advertising.start(ADV_TIMEOUT);      // Stop advertising entirely after ADV_TIMEOUT seconds 
// }
