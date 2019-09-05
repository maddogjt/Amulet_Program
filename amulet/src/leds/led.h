#pragma once

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

void led_set_brightness(LedBrightness brightness);
void led_next_brightness();
void led_refresh_brightness();
void led_override_brightness(bool over);