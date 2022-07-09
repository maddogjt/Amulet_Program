

#include "system_sleep.h"

#include <Arduino.h>

#include "../leds/led.h"
#include "../communication/scanning.h"

void power_off()
{
	scanning_stop();

#define DFU_MAGIC_IGNORE_PIN			0xC6
    nrf_gpio_cfg_sense_input(PIN_DFU, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
	nrf_gpio_cfg_sense_input(PIN_RESET, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);

	NRF_POWER->GPREGRET = DFU_MAGIC_IGNORE_PIN;
    NRF_POWER->SYSTEMOFF = 1;
}

void systemSleep()
{
	Serial.println("Going to sleep now");

	digitalWrite(LED_BUILTIN, LED_STATE_ON);
	delay(500);

	led_set_brightness(LedBrightness::Off);
	digitalWrite(LED_BUILTIN, !LED_STATE_ON);

	power_off();
}