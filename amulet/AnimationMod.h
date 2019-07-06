#pragma once

#define ARDUINO_GENERIC
#include <FastLED.h>
#undef ARDUINO_GENERIC

int getMaskName(char *buf, const uint8_t mask_number);
void initMask(const uint8_t mask_number);
void getMask(float *mask, const uint8_t len, const uint8_t mask_number, int frame);

int getFilterName(char *buf, const uint8_t filter_number);
void filterLEDs(CRGB *leds, float *mask, const uint8_t len, const uint8_t filter);
