/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Blink
*/
#include <FastLED.h>
#include <nrf.h>
#include <nrf_pwm.h>  // for Clockless
#include <nrf_nvic.h> // for Clockless / anything else using interrupts

uint16_t gSequenceBuffer[16] = { 700, 700, 200, 600, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


static void playTone(uint16_t freq, uint8_t intensity)
{
    NRF_PWM_Type *pwm = NRF_PWM2;

    FastPin<A4>::setOutput();
    FastPin<A4>::lo();
    FastPin<A5>::setOutput();
    FastPin<A5>::lo();

//    uint16_t gFreq = 1000;
    uint16_t counterLength = 1000000 / freq*2;

    // Pins must be set before enabling the peripheral
    pwm->PSEL.OUT[0] = FastPin<A4>::nrf_pin();
    pwm->PSEL.OUT[1] = NRF_PWM_PIN_NOT_CONNECTED;
    pwm->PSEL.OUT[2] = FastPin<A5>::nrf_pin();
    pwm->PSEL.OUT[3] = NRF_PWM_PIN_NOT_CONNECTED;
    nrf_pwm_enable(pwm);
    nrf_pwm_configure(pwm, NRF_PWM_CLK_1MHz, NRF_PWM_MODE_UP, counterLength);
    nrf_pwm_decoder_set(pwm, NRF_PWM_LOAD_GROUPED, NRF_PWM_STEP_AUTO);

    // clear any prior shorts / interrupt enable bits
    nrf_pwm_shorts_set(pwm, 0);
    nrf_pwm_int_set(pwm, 0);
    // clear all prior events
    nrf_pwm_event_clear(pwm, NRF_PWM_EVENT_STOPPED);
    nrf_pwm_event_clear(pwm, NRF_PWM_EVENT_SEQSTARTED0);
    nrf_pwm_event_clear(pwm, NRF_PWM_EVENT_SEQSTARTED1);
    nrf_pwm_event_clear(pwm, NRF_PWM_EVENT_SEQEND0);
    nrf_pwm_event_clear(pwm, NRF_PWM_EVENT_SEQEND1);
    nrf_pwm_event_clear(pwm, NRF_PWM_EVENT_PWMPERIODEND);
    nrf_pwm_event_clear(pwm, NRF_PWM_EVENT_LOOPSDONE);

    gSequenceBuffer[0] = counterLength/ 2;
    gSequenceBuffer[1] = counterLength;// / 2 | 0x8000;
    gSequenceBuffer[2] = 0;
    gSequenceBuffer[3] = 0;
    gSequenceBuffer[4] = (counterLength / 2 + counterLength / 4);
    gSequenceBuffer[5] = (counterLength / 2 - counterLength / 4) | 0x8000;
    gSequenceBuffer[6] = counterLength;
    gSequenceBuffer[7] = counterLength / 2;

    // config is easy, using SEQ0, no loops...
    nrf_pwm_sequence_t sequenceConfig;
    sequenceConfig.values.p_common = &(gSequenceBuffer[0]);
    sequenceConfig.length = 2;
    sequenceConfig.repeats = 0;   // send the data once, and only once
    sequenceConfig.end_delay = 0; // no extra delay at the end of SEQ[0] / SEQ[1]
    nrf_pwm_sequence_set(pwm, 0, &sequenceConfig);
    nrf_pwm_sequence_set(pwm, 1, &sequenceConfig);
    nrf_pwm_loop_set(pwm, 0);

    nrf_pwm_event_clear(pwm, NRF_PWM_EVENT_STOPPED);

    nrf_pwm_task_trigger(pwm, NRF_PWM_TASK_SEQSTART0);
}

void stopTone() {
    NRF_PWM_Type *pwm = NRF_PWM2;
    nrf_pwm_disable(pwm);
    FastPin<A0>::lo();
    FastPin<A1>::lo();
}

constexpr int led_pins[4] = {25, 27, 26, 30}; // R,G,B,Y
constexpr int button_pins[4] = {15, 16, 11, 7}; // R,G,B,Y
constexpr int freqs[4] = {310, 415, 209, 252};
constexpr int freq_bad = 42;
constexpr int duration_bad = 1500;

constexpr int duration_level[3] = {420, 320, 220};
constexpr int duration_between = 50;

constexpr int seq[] = {0, 2, 3, 2, 2, 2, 1};


// the setup function runs once when you press reset or power the board
void setup()
{
    Serial.begin(115200);

    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);

    for (auto p : led_pins) {
        pinMode(p, OUTPUT);
        digitalWrite(p, 1);
    }

    for (auto p : button_pins)
    {
        pinMode(p, INPUT_PULLUP);
        digitalWrite(p, 1);
    }

    //initPwm();
    //configurePwmSequence();
}


// the loop function runs over and over again forever
void loop()
{
    for (auto v : seq) {
        digitalWrite(led_pins[v], 0);
        playTone(freqs[v], 255);
        delay(duration_level[0]);
        digitalWrite(led_pins[v], 1);
        stopTone();
        delay(duration_between);

    }
    // for (int i = 0; i < 4; i++) {
    //     int v = digitalRead(button_pins[i]);
    //         digitalWrite(led_pins[i], digitalRead(button_pins[i]));
    //     //     Serial.print("pin ");
    //     // Serial.print(i);
    //     // Serial.print(": ");
    //     // Serial.println(v);
        
    // }

    delay(10);
    // digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
    // playTone(415, 255);
    // delay(1000);                 // wait for a second
    // digitalWrite(LED_BUILTIN, LOW);  // turn the LED off by making the voltage LOW
    // stopTone();
    // playTone(310, 255);
    // delay(1000);                     // wait for a second
    // stopTone();
    // digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
    // playTone(252, 255);
    // delay(1000);                    // wait for a second
    // digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
    // stopTone();
    // playTone(209, 255);
    // delay(1000); // wait for a second
    // stopTone();
    // playTone(42, 255);
    // delay(1000); // wait for a second
    // stopTone();
    // FastPin<A0>::hi();
    // delay(2000);
}