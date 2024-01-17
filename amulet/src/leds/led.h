#pragma once
#include <cstdint>

enum class LedBrightness
{
	Low,
	Medium,
	High,
	Off,
	Count,
};

void led_setup();
void led_loop(int step);

LedBrightness led_get_brightness();
void led_set_brightness(LedBrightness brightness);
void led_next_brightness();
void led_refresh_brightness();
void led_override_brightness(bool over);
void led_override_brightness_value(uint8_t brightness);