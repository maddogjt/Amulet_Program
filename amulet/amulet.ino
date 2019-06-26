#include <JC_Button.h>
#include <FastLED.h>

#include "globals.h"
#include "led.h"
#include "ble.h"
#include "signal.h"

Button dfuButton(PIN_DFU, 25, true, true);

constexpr int singleClick = 1;
constexpr int doubleClick = 2;
constexpr int longHold = 3;

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

// NOTE! If (when) we get asserts (crashes) it's probably because we have violated the timing needs of the BLE layer
// see Sigurd's response here: https://devzone.nordicsemi.com/f/nordic-q-a/33961/s132-5-1-0-sd-assert-error

void systemSleep();

void setup()
{
  // Set pin modes
  pinMode(28, INPUT_PULLUP);      // @Jason is this important?
  pinMode(LED_BUILTIN, OUTPUT);

  // Start Serial 
  Serial.begin(115200);
  Serial.println("Amulet startup");

  // Turn on LED power rail
  pinMode(PIN_RGB_LED_PWR, OUTPUT);
  digitalWrite(PIN_RGB_LED_PWR, RGB_LED_PWR_ON);

  // Start reading the DFU button so we can trigger off long and short presses
  dfuButton.begin();

  // Custom setup for the LED and BLE components
  led_setup();
  ble_setup();
}

int step = 0;
void loop()
{
  // LOG_LV2("LOOP", "Loop start");
  led_loop( step );
  // yield();
  ble_loop( step );
  // yield();
  signal_loop( step );


  dfuButton.read();
  if (dfuButton.pressedFor(2000)) {
    // Long press means power down.
    systemSleep();
  } else if (dfuButton.wasReleased())
  { 
    // Currently undefined function.
    Serial.println("DFU button released");
  }

  // Advance the step counter and enforce the max framerate
  delay( 1000 / FRAMERATE );
  // FastLED.delay(1000 / FRAMERATE);
  step++;
  // LOG_LV2("LOOP", "Loop end");

}

void systemSleep()
{
    Serial.println("Going to sleep now");

    digitalWrite(PIN_RGB_LED_PWR, !RGB_LED_PWR_ON);
    digitalWrite(LED_BUILTIN, !LED_STATE_ON);

    systemOff(29, 0);
}
