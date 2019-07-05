#pragma once

#include <Arduino.h>
#include "Adafruit_LittleFS.h"

typedef struct __attribute__ ((packed)) 
{
	int16_t signature_;
	uint8_t version_;
	uint8_t brightness_[3];
	int8_t txPower_;

	uint8_t pad_[12];
} GlobalSettings;

static_assert(sizeof(GlobalSettings)==19, "");


typedef struct __attribute__ ((packed)) 
{
	int16_t signature_;
	uint8_t version_;
	uint8_t pad_[61];
} LocalSettings;

static_assert(sizeof(LocalSettings)==64, "");


void write_global_settings();
void write_local_settings();
void settings_init();

extern GlobalSettings globalSettings_;
extern LocalSettings localSettings_;