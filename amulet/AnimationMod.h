#pragma once

#define ARDUINO_GENERIC
#include <FastLED.h>
#undef ARDUINO_GENERIC

void modLEDs(CRGB *leds, const uint8_t len, const uint8_t mod);
