
#include <FastLED.h>

#include "Animation.h"
#include "animations.h"
#include "signal.h"
#include "globals.h"
#include "Startup.h"
#include "settings.h"

FASTLED_USING_NAMESPACE

#define DATA_PIN PIN_RGB_LEDS
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define BRIGHTNESS 8

anim_config_t ambientAnimation = {};

void led_set_ambient_animation(const anim_config_t &anim);

// returns true if animation was changed
void set_animation_from_signal(Signal *s)
{
	if (s != nullptr)
	{
		anim_config_t pattern;
		VERIFY_STATIC(sizeof(pattern) <= kMaxPayloadLen);
		memcpy(&pattern, s->_scan._data, sizeof(anim_config_t));
		if (!matches_current_animation(pattern))
		{
			LOG_LV1("LED", "Starting Animation from scan (%s)", get_advertisement_type_name((AdvertisementType)s->_scan.signal_type));
			start_animation(pattern);
		}
		else if (!(ambientAnimation == pattern))
		{
			// It is our current pattern, but it is not our ambient pattern,
			// Maybe it should become our ambient pattern
			if (s->_scan.signal_type == (uint8_t)AdvertisementType::Runic &&
				s->_seenCount >= globalSettings_.runeSeenCountThreshold_)
			{
				LOG_LV1("LED", "Setting Ambient Animation from rune");
				localSettings_.startupConfig_.pattern = pattern;
				write_local_settings();
				led_set_ambient_animation(pattern);
			}
		}
	}
	else
	{
		if (!matches_current_animation(ambientAnimation))
		{
			start_animation(ambientAnimation);
		}
	}
}

void run_power_animation() {
	if (!matches_current_animation(localSettings_.powerPattern_))
	{
		start_animation(localSettings_.powerPattern_);
	}
}

void led_set_ambient_animation(const anim_config_t &anim)
{
	ambientAnimation = anim;
	set_animation_from_signal(nullptr);
}

anim_config_t led_get_ambient_animation()
{
	return ambientAnimation;
}

extern CRGB gLeds[RGB_LED_COUNT];

void led_setup()
{
	FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(gLeds, RGB_LED_COUNT).setCorrection(TypicalLEDStrip);
}

extern bool powerIsAdvertising;
void run_power_animation();

void led_loop(int step)
{
	Signal *signal = nullptr;
	// Update the LED pattern based on bluetooth signals every 500ms
	EVERY_N_MILLISECONDS(globalSettings_.animationUpdateTimer_)
	{
		if (powerIsAdvertising) {
			run_power_animation();
		}
		else if (isAmulet())
		{
			signal = current_top_signal();
			set_animation_from_signal(signal);
		}
	}

	if (gMode == AMULET_MODE_RUNE)
	{
		signal = current_top_signal();
		int topRSSI = signal == nullptr ? -128 : signal->_scan.rssi;
		float strength = normalizeRSSI(topRSSI);
		FastLED.setBrightness(10 + strength * 180);
	}

	// Step the currentAnimation
	step_animation(signal);
	FastLED.show();
}
