#pragma once
#include <cstdint>

enum class Anim : uint8_t {
#define DEFINE_ANIM(name) name,
#include "animation_list.hpp"
#undef DEFINE_ANIM
		Count,
};