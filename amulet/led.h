#pragma once

#include "animations.h"

void led_setup();
void led_loop(int step);

void led_set_ambient_animation(const anim_config_t &anim);