#define ARDUINO_GENERIC
#include <FastLED.h>
#undef ARDUINO_GENERIC

#include "Animation.h"
#include "animations.h"
#include "signal.h"
#include "globals.h"
#include "Startup.h"

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
			if (s->_scan.signal_type == (uint8_t)AdvertisementType::Runic)
			{
				LOG_LV1("LED", "Setting Ambient Animation from rune");
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

// -----------------------
//
// TEST PATTERN CODE
//
// -----------------------
int test_pattern_idx = 0;
const int test_pattern_count = 12;

void led_show_test_pattern_by_index(int idx)
{
	switch (idx)
	{
	case 0:
		start_animation(Anim::AnimSolidHue, 0, 5);
		break;
	case 1:
		start_animation(Anim::AnimRainbow, 0, 2);
		break;
	case 2:
		start_animation(Anim::AnimTwister, 0, 0);
		break;
	case 3:
		start_animation(Anim::AnimFlame, 0, 0);
		break;
	case 4:
		start_animation(Anim::AnimDebugInfo, 5, 0);
		break;
	case 5:
		start_animation(Anim::AnimCylon, 5, 0);
		break;
	case 6:
		start_animation(Anim::AnimJuggle, 5, 0);
		break;
	case 7:
		start_animation(Anim::AnimSinelon, 5, 2);
		break;
	case 8:
		start_animation(Anim::AnimBPM, 5, 2);
		break;
	case 9:
		start_animation(Anim::AnimConfetti, 0, 0);
		break;
	case 10:
		start_animation(Anim::AnimBallRaster, 0, 0);
		break;
	case 11:
		start_animation(Anim::AnimRainbowRaster, 0, 0);
		break;
	default:
		break;
	}

	led_test_mode = true;
}

void led_show_test_pattern(bool show)
{
	led_test_mode = show;
	if (led_test_mode)
	{
		test_pattern_idx = 0;
		led_show_test_pattern_by_index(0);
	}
}

bool led_test_has_next_pattern()
{
	return test_pattern_idx + 1 < test_pattern_count;
}

void led_show_next_test_pattern()
{
	test_pattern_idx = (test_pattern_idx + 1) % test_pattern_count;
	led_show_test_pattern_by_index(test_pattern_idx);
}
