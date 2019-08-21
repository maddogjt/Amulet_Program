#pragma once

#include <Arduino.h>
typedef enum
{
    AMULET_BRIGHTNESS_LOW,
    AMULET_BRIGHTNESS_MEDIUM,
    AMULET_BRIGHTNESS_HIGH,
    AMULET_BRIGHTNESS_OFF,
    AMULET_BRIGHTNESS_MODE_COUNT,
} amulet_brightness_mode_t;

void setBrightnessMode(amulet_brightness_mode_t mode);
void nextBrightnessMode();
void refreshBrightness();