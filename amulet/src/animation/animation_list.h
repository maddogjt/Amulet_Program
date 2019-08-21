#pragma once
#include <cstdint>

enum class Anim : uint8_t {
#define DEFINE_ANIM(name) name,
#include "../../AnimList.hpp"
#undef DEFINE_ANIM
		Count,
};