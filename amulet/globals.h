#pragma once

// process scan -> signal array
// process signal (signal / strength array) -> top signal + state data
// top signal + state data -> new advertisement + LED pattern

#define FRAMERATE 120

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))
