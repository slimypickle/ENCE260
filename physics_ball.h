// Josh Craythorne & Jamie Pedersen

#ifndef PHYSICS_BALL_H
#define PHYSICS_BALL_H

#include <stdbool.h>
#include <stdint.h>

/* Update result for a ball step. */
typedef enum {
    BALL_UPDATE_MISS,
    BALL_UPDATE_TRANSFER,
    BALL_UPDATE_NORMAL,
} ball_update_t;

/* Ball direction of travel. */
typedef enum {
    BALL_DIR_NE,
    BALL_DIR_SE,
    BALL_DIR_SW,
    BALL_DIR_NW,
} ball_dir_t;

/* Ball state (game coordinates: x = left->right, y = top->bottom). */
typedef struct {
    uint8_t    x;
    uint8_t    y;
    ball_dir_t dir;
    bool       has_ball;
} ball_state_t;

/* Advance ball state given current paddle center x-position. */
ball_update_t ball_update_state(ball_state_t* state, uint8_t paddle_position);

/* Mirror the direction when transferring to the opponent’s display. */
ball_dir_t ball_mirror_dir(ball_dir_t dir);

#endif
