
#ifndef AMULET_ANIMATION_H
#define AMULET_ANIMATION_H

typedef void (*init_func)();
typedef void (*step_func)(int frame);

typedef struct
{
    init_func init;
    step_func step;
    int p1;
    int p2;
} animation_t;

typedef enum
{
    ANIMATION_SOLID_HUE,
    ANIMATION_RAINBOW,
    ANIMATION_TWISTER,
    ANIMATION_FLAME,
    ANIMATION_DEBUG_INFO,
    ANIMATION_CYLON,
    ANIMATION_JUGGLE,
    ANIMATION_SINELON,
    ANIMATION_BPM,
    ANIMATION_CONFETTI,
} animation_name_type_t;

void animation_for_name( animation_t &out, animation_name_type_t name, int p1, int p2);
bool animations_are_equal( animation_t const &p1, animation_t const &p2);

#endif
