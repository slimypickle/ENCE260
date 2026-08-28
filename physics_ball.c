// Josh Craythorne & Jamie Pedersen

#include "physics_ball.h"
#include "app_config.h"
#include <stdbool.h>
#include <stdint.h>

static inline bool ball_is_north(ball_dir_t d) { return d == BALL_DIR_NE || d == BALL_DIR_NW; }
static inline bool ball_is_south(ball_dir_t d) { return !ball_is_north(d); }
static inline bool ball_is_east(ball_dir_t d)  { return d == BALL_DIR_NE || d == BALL_DIR_SE; }
static inline bool ball_is_west(ball_dir_t d)  { return !ball_is_east(d); }

/* Reflect off a side wall. */
static ball_dir_t ball_deflect_wall(ball_dir_t d)
{
    switch (d) {
    case BALL_DIR_NE: return BALL_DIR_NW;
    case BALL_DIR_SE: return BALL_DIR_SW;
    case BALL_DIR_SW: return BALL_DIR_SE;
    case BALL_DIR_NW: return BALL_DIR_NE;
    }
    return d;
}

ball_update_t ball_update_state(ball_state_t* state, uint8_t paddle_position)
{
    if (!state->has_ball) {
        return BALL_UPDATE_NORMAL;
    }

    /* If already at bottom row, next south step would miss. */
    if (state->y == (uint8_t)(GAME_HEIGHT - 1)) {
        return BALL_UPDATE_MISS;
    }

    /* Reflect on left/right walls. */
    if ((state->x == 0 && ball_is_west(state->dir)) ||
        (state->x == (uint8_t)(GAME_WIDTH - 1) && ball_is_east(state->dir))) {
        state->dir = ball_deflect_wall(state->dir);
    }

    if (state->y == (uint8_t)(GAME_HEIGHT - 2) && ball_is_south(state->dir)) {
        /* Paddle is 5 pixels wide: center at paddle_position, span = [p-2, p-1, p, p+1, p+2]. */
        int8_t dx = (int8_t)state->x - (int8_t)paddle_position;

        if (dx >= -2 && dx <= 2) {
            /* Hit the paddle: choose outgoing direction by where it struck. */
            if (dx > 0) {
                /* Left side of paddle -> send up-right. */
                state->dir = BALL_DIR_NE;
            } else if (dx < 0) {
                /* Right side of paddle -> send up-left. */
                state->dir = BALL_DIR_NW;
            } else {
                /* Center hit: mirror vertically (keep horizontal sense). */
                state->dir = (state->dir == BALL_DIR_SE) ? BALL_DIR_NE : BALL_DIR_NW;
            }
        }
        /* Else: miss — no change here; next row down becomes MISS. */
    }
    /* Step X first so transfer appears at a distinct X on the remote side. */
    state->x = (uint8_t)(state->x + (ball_is_east(state->dir) ? 1 : (uint8_t)-1));

    /* Transferring off the top edge. */
    if (state->y == 0 && ball_is_north(state->dir)) {
        return BALL_UPDATE_TRANSFER;
    }

    /* Step Y. */
    state->y = (uint8_t)(state->y + (ball_is_south(state->dir) ? 1 : (uint8_t)-1));

    return BALL_UPDATE_NORMAL;
}

ball_dir_t ball_mirror_dir(ball_dir_t d)
{
    switch (d) {
    case BALL_DIR_NE: return BALL_DIR_SW;
    case BALL_DIR_SE: return BALL_DIR_NW;
    case BALL_DIR_SW: return BALL_DIR_NE;
    case BALL_DIR_NW: return BALL_DIR_SE;
    }
    return d;
}
