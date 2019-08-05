#pragma once

#include "animations.h"

void led_setup();
void led_loop(int step);

void led_set_ambient_animation(const anim_config_t &anim);
anim_config_t led_get_ambient_animation();
void led_show_test_pattern(bool show);
void led_show_next_test_pattern();
bool led_test_has_next_pattern();
