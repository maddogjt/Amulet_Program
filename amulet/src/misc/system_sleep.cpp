

#include "system_sleep.h"

#include <Arduino.h>

#include "../leds/led.h"
#include "../communication/scanning.h"

void power_off()
{
	// Make sure all LEDs are powered off
	digitalWrite(LED_BUILTIN, !LED_STATE_ON);
	digitalWrite(PIN_RGB_LED_PWR, !RGB_LED_PWR_ON);

	// Make sure scanning is stopped
	scanning_stop();

	// Set DFU & RESET to wake the device
	nrf_gpio_cfg_sense_input(PIN_DFU, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
	nrf_gpio_cfg_sense_input(PIN_RESET, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);

	// Tell the bootloader not to run DFU if DFU pressed, since it will be used for wakeup
#define DFU_MAGIC_IGNORE_PIN			0xC6
	NRF_POWER->GPREGRET = DFU_MAGIC_IGNORE_PIN;

	// power off
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