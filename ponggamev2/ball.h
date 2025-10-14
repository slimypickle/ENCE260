#ifndef BALL_H
#define BALL_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    Miss,
    Transfer,
    Normal,
} BallUpdateResult;

/**
 * Represents the possible directions of travel of the ball
 */
typedef enum : uint8_t {
    NE,
    SE,
    SW,
    NW,
} Direction;

typedef struct {
    /**
     * X coordinate of the ball, going left to right relative to player view
     */
    uint8_t x;
    /**
     * Y coordinate of the ball, going top to bottom relative to player view
     */
    uint8_t y;
    Direction direction;

    bool has_ball;
} BallState;

/**
 * Mutates the current ball state based upon the previous state, and the
 * coordinates of the paddle
 *
 * Returns an enum representing whether or not the overall game state should
 * change
 */
BallUpdateResult update_ball_state(BallState *state, uint8_t paddle_position);

/**
 * Determines the direction of a ball from the opposing player
 */
Direction mirror_direction(Direction direction);

#endif
