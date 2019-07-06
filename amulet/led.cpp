#define ARDUINO_GENERIC
#include <FastLED.h>
#undef ARDUINO_GENERIC

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

bool show_ambient = true;
bool led_test_mode = false;
animPattern ambientAnimation = {};

void led_set_ambient_animation(const animPattern &anim);

// returns true if animation was changed
void set_animation_from_signal(Signal *s)
{
	if (s != nullptr)
	{
		animPattern pattern;
		VERIFY_STATIC(sizeof(pattern) <= MAX_MFD_DATA_LEN);
		memcpy(&pattern, s->_scan._data, sizeof(animPattern));
		if (!matches_current_animation(pattern))
		{
			// Todo: Pull the seen count threshold from settings
			if (s->_scan.signal_type == (uint8_t)AdvertisementType::Runic &&
				s->_seenCount >= globalSettings_.runeSeenCountThreshold_)
			{
				LOG_LV1("LED", "Setting Ambient Animation from rune");
				localSettings_.startupConfig_.pattern = pattern;
				write_local_settings();
				led_set_ambient_animation(pattern);
			}
			LOG_LV1("LED", "Starting Animation from scan");
			start_animation(pattern);
		}
	}
	else
	{
		show_ambient = true;
		LOG_LV1("LED", "Going to show new ambient if not same");
		if (!matches_current_animation(ambientAnimation))
		{
			LOG_LV1("LED", "Starting Ambient Animation");
			start_animation(ambientAnimation);
		}
	}
}

void led_set_ambient_animation(const animPattern &anim)
{
	ambientAnimation = anim;
	set_animation_from_signal(nullptr);
}

animPattern led_get_ambient_animation()
{
	return ambientAnimation;
}


void led_setup()
{
	FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(gLeds, RGB_LED_COUNT).setCorrection(TypicalLEDStrip);
}

void choose_pattern_by_signal()
{
	LOG_LV1("LED", "choose_pattern_by_signal");
	Signal *signal = current_top_signal();
	set_animation_from_signal(signal);
}

void led_loop(int step)
{
	// Update the LED pattern based on bluetooth signals every 500ms
	if (isAmulet() && !led_test_mode && step % 12 == 0)
	{
		choose_pattern_by_signal();
	}

	// Step the currentAnimation
	step_animation();
	FastLED.show();
}
