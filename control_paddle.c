// Josh Craythorne & Jamie Pedersen

#include "control_paddle.h"
#include "app_config.h"
#include "navswitch.h"
#include <stdint.h>

/* Clamp helper. */
static inline uint8_t clamp_u8(uint8_t v, uint8_t lo, uint8_t hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

/* Expects navswitch_update() to be called in the main input task each tick. */
void update_paddle_position(uint8_t* paddle_position)
{
    if (!paddle_position) {
        return;
    }

    /* Move “up”/“down” in game X (centered), clamped away from extreme walls. */
    uint8_t pos = *paddle_position;

    if (navswitch_push_event_p(NAVSWITCH_NORTH)) {
        pos = (uint8_t)(pos + 1);
    }
    if (navswitch_push_event_p(NAVSWITCH_SOUTH)) {
        pos = (uint8_t)(pos - 1);
    }

    pos = clamp_u8(pos, 1, (uint8_t)(GAME_WIDTH - 2));
    *paddle_position = pos;
}
