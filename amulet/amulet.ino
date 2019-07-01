#include <JC_Button.h>
#define ARDUINO_GENERIC
#include <FastLED.h>
#undef ARDUINO_GENERIC

#include "globals.h"
#include "led.h"
#include "ble.h"
#include "signal.h"

Button dfuButton(PIN_DFU, 25, true, true);


#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif



// NOTE! If (when) we get asserts (crashes) it's probably because we have violated the timing needs of the BLE layer
// see Sigurd's response here: https://devzone.nordicsemi.com/f/nordic-q-a/33961/s132-5-1-0-sd-assert-error
// or something to do with FASTLED interrupts https://github.com/FastLED/FastLED/wiki/Interrupt-problems

typedef enum {
  AMULET_MODE_AMULET,
  AMULET_MODE_BEACON,
  AMULET_MODE_LED_TEST,
  AMULET_MODE_COUNT,
} amulet_mode_t;

amulet_mode_t mode = AMULET_MODE_AMULET;
void nextMode();
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
  ble_loop( step );
  signal_loop( step );

  dfuButton.read();
  if (dfuButton.pressedFor(2000)) {
    // Long press means power down.
    systemSleep();
  } else if (dfuButton.wasReleased())
  { 
    Serial.println("DFU button released, switching mode");
    switch( mode ) {
      case AMULET_MODE_AMULET:
        nextMode();
        break;
      case AMULET_MODE_BEACON:
        nextMode();
        break;
      case AMULET_MODE_LED_TEST:
        // Instead of going to the next mode, go to the next test pattern if there is one.
        if( led_test_has_next_pattern() ) {
          led_show_next_test_pattern();
        } else {
          led_show_test_pattern(false);
          nextMode();
        }
        break;
      case AMULET_MODE_COUNT:
      default:
        break;
    }
  }

  // Advance the step counter and enforce the max framerate
  // delay( 1000 / FRAMERATE );
  FastLED.delay(1000 / FRAMERATE);
  step++;
  // LOG_LV2("LOOP", "Loop end");
}

void nextMode()
{
  mode = (amulet_mode_t)(((int)mode + 1) % (int)AMULET_MODE_COUNT);
  switch( mode ) {
    case AMULET_MODE_AMULET:
    case AMULET_MODE_BEACON:
      break;
    case AMULET_MODE_LED_TEST:
      digitalWrite(LED_BUILTIN, LED_STATE_ON);
      led_show_test_pattern(true);
      break;
    case AMULET_MODE_COUNT:
      // shouldn't be here
      break;
  }
  // Turn on the builtin if we are not an amulet.
  digitalWrite(LED_BUILTIN, mode==AMULET_MODE_AMULET ? !LED_STATE_ON : LED_STATE_ON);
}

void systemSleep()
{
    Serial.println("Going to sleep now");

    digitalWrite(PIN_RGB_LED_PWR, !RGB_LED_PWR_ON);
    digitalWrite(LED_BUILTIN, !LED_STATE_ON);

    systemOff(29, 0);
}
