#pragma once

#include <FastLED.h>

int getMaskName(char *buf, const uint8_t mask_number);
int getFilterName(char *buf, const uint8_t filter_number);

void initMask(const uint8_t mask_number);
void maskAndFilter(CRGB *leds, const uint8_t len, const uint8_t filter_number, int frame);
void maskAndFilter(CRGB *leds, const uint8_t len, const uint8_t filter_number, int frame, int alpha);
