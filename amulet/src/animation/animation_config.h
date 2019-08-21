#pragma once
#include <cstdint>

#include "animation_list.h"
#include "animation_modifiers.h"
#include "animation_overlay.h"

struct anim_config_t
{
	Anim anim_;
	uint8_t color1_;
	uint8_t color2_;
	uint8_t speed_;
	AnimationModifier modifiers_;
	uint8_t extra0_;
	uint8_t extra1_;
	Anim overlay_;
	OverlayFilter filter_;
	bool operator==(const anim_config_t &p) const
	{
		return anim_ == p.anim_ &&
			   color1_ == p.color1_ && color2_ == p.color2_ &&
			   overlay_ == p.overlay_ && filter_ == p.filter_ &&
			   speed_ == p.speed_ && modifiers_ == p.modifiers_ &&
			   extra0_ == p.extra0_ && extra1_ == p.extra1_;
		;
	}
};
