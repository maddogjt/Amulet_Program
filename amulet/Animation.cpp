
#include "animations.h"

bool animations_are_equal( animation_t const &p1, animation_t const &p2)
{
    return  p1.init == p2.init && 
            p1.step == p2.step && 
            p1.p1 == p2.p1 && 
            p1.p2 == p2.p2;
}

void animation_for_name( animation_t &out, animation_name_type_t name, int p1, int p2)
{
    out.p1 = p1;
    out.p2 = p2;
    switch (name)
    {
    case ANIMATION_SOLID_HUE:
        out.init = SolidHue_init;
        out.step = SolidHue_step;
        break;
    case ANIMATION_RAINBOW:
        out.init = Rainbow_init;
        out.step = Rainbow_step;
        break;
    case ANIMATION_TWISTER:
        out.init = Twister_init;
        out.step = Twister_step;
        break;
    case ANIMATION_FLAME:
        out.init = Flame_init;
        out.step = Flame_step;
        break;
    case ANIMATION_DEBUG_INFO:
        out.init = DebugInfo_init;
        out.step = DebugInfo_step;
        break;
    case ANIMATION_CYLON:
        out.init = Cylon_init;
        out.step = Cylon_step;
        break;
    case ANIMATION_JUGGLE:
        out.init = Juggle_init;
        out.step = Juggle_step;
        break;
    case ANIMATION_SINELON:
        out.init = Sinelon_init;
        out.step = Sinelon_step;
        break;
    case ANIMATION_BPM:
        out.init = Bpm_init;
        out.step = Bpm_step;
        break;
    case ANIMATION_CONFETTI:
        out.init = Confetti_init;
        out.step = Confetti_step;
        break;
    default:
        LOG_LV1("ANIM", "Bad animation name");
        break;
    }
}