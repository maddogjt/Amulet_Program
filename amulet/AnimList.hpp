#ifdef DO_INCLUDES
#undef DO_INCLUDES
#include "animations/AnimSolidHue.h"
#include "animations/AnimRainbow.h"
#include "animations/AnimTwister.h"
#include "animations/AnimFlame.h"
#include "animations/AnimBurn.h"
#include "animations/AnimDebugInfo.h"
#include "animations/AnimCylon.h"
#include "animations/AnimJuggle.h"
#include "animations/AnimPsyco.h"
#include "animations/AnimSinelon.h"
#include "animations/AnimBPM.h"
#include "animations/AnimConfetti.h"
#include "animations/AnimShadow.h"
#include "animations/AnimPrism.h"
#include "animations/AnimBounce.h"
#include "animations/AnimInNOut.h"
#include "animations/AnimJump.h"
#include "animations/AnimOrbit.h"
#include "animations/AnimTwinkle.h"
#else

#ifndef DEFINE_ANIM
#define CLEAR_DEFINE_ANIM
#define DEFINE_ANIM(...)
#endif

DEFINE_ANIM(AnimSolidHue)
DEFINE_ANIM(AnimRainbow)
DEFINE_ANIM(AnimTwister)
DEFINE_ANIM(AnimBurn)

DEFINE_ANIM(AnimJump)
DEFINE_ANIM(AnimOrbit)
DEFINE_ANIM(AnimTwinkle)
DEFINE_ANIM(AnimBounce)
DEFINE_ANIM(AnimInNOut)

DEFINE_ANIM(AnimDebugInfo)
DEFINE_ANIM(AnimCylon)
DEFINE_ANIM(AnimJuggle)
DEFINE_ANIM(AnimPsyco)
DEFINE_ANIM(AnimSinelon)
DEFINE_ANIM(AnimBPM)
DEFINE_ANIM(AnimConfetti)
DEFINE_ANIM(AnimShadow)
DEFINE_ANIM(AnimPrism)

#ifdef CLEAR_DEFINE_ANIM
#undef DEFINE_ANIM
#endif
#endif
