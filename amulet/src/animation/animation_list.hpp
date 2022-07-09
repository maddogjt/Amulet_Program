#ifdef DO_INCLUDES
#undef DO_INCLUDES
#include "../../animations/AnimSolidHue.h"

#include "../../animations/AnimDisplayInt.h"

#include "../../animations/AnimRainbow.h"
#include "../../animations/AnimTwister.h"

#include "../../animations/AnimBurn.h"
#include "../../animations/AnimCylon.h"
#include "../../animations/AnimJuggle.h"
#include "../../animations/AnimPsyco.h"

#include "../../animations/AnimSinelon.h"
#include "../../animations/AnimBPM.h"
#include "../../animations/AnimConfetti.h"
#include "../../animations/AnimShadow.h"

#include "../../animations/AnimPrism.h"
#include "../../animations/AnimBounce.h"
#include "../../animations/AnimInNOut.h"
#include "../../animations/AnimJump.h"

#include "../../animations/AnimOrbit.h"
#include "../../animations/AnimTwinkle.h"
#include "../../animations/AnimFlame.h"


// #include "../../animations/AnimDebugInfo.h"

// #include "../../animations/AnimSimon.h"
// #include "../../animations/AnimSafe.h"
// #include "../../animations/AnimPhotoKey.h"
// #include "../../animations/AnimScoreboard.h"
// #include "../../animations/AnimVictory.h"
#else

#ifndef DEFINE_ANIM
#define CLEAR_DEFINE_ANIM
#define DEFINE_ANIM(...)
#endif

// Group 1
DEFINE_ANIM(AnimSolidHue)
DEFINE_ANIM(AnimPrism)
DEFINE_ANIM(AnimTwister)
DEFINE_ANIM(AnimBurn)

// Group 2
DEFINE_ANIM(AnimJump)
DEFINE_ANIM(AnimOrbit)
DEFINE_ANIM(AnimTwinkle)
DEFINE_ANIM(AnimInNOut)

// Group 3
DEFINE_ANIM(AnimCylon)
DEFINE_ANIM(AnimJuggle)
DEFINE_ANIM(AnimPsyco)
DEFINE_ANIM(AnimShadow)

// Group 4
DEFINE_ANIM(AnimBPM)
DEFINE_ANIM(AnimConfetti)

// Extra
DEFINE_ANIM(AnimSinelon) // similar to cylon but not interests
DEFINE_ANIM(AnimRainbow) // this is good, should be on the list.
DEFINE_ANIM(AnimBounce) // like sninelon without the trails
DEFINE_ANIM(AnimDisplayInt)
DEFINE_ANIM(AnimVictory)

// DEFINE_ANIM(AnimDebugInfo)
// DEFINE_ANIM(AnimSimon)
// DEFINE_ANIM(AnimSafe)
// DEFINE_ANIM(AnimPhotoKey)
// DEFINE_ANIM(AnimScoreboard)

#ifdef CLEAR_DEFINE_ANIM
#undef DEFINE_ANIM
#endif
#endif
