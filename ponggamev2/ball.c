#include "ball.h"
#include "game.h"
#include <stdbool.h>
#include <stdint.h>

bool is_north(Direction direction)
{
    return direction == NE || direction == NW;
}

bool is_south(Direction direction) { return !is_north(direction); }

bool is_east(Direction direction) { return direction == NE || direction == SE; }

bool is_west(Direction direction) { return !is_east(direction); }

/**
 * Determines the next direction based on current direction when hitting a wall
 */
Direction deflect_wall(Direction direction)
{
    switch (direction) {
        case NE:
            return NW;
        case SE:
            return SW;
        case SW:
            return SE;
        case NW:
            return NE;
    }
}

BallUpdateResult update_ball_state(BallState *state, uint8_t paddle_position)
{
    if (!state->has_ball) {
        return Normal;
    }

    if (state->y == GAME_HEIGHT - 1) {
        // The ball is in the bottom row, it can only get here by traveling
        // down, so this state inccurs a Miss
        return Miss;
    }

    if ((state->x == 0 && is_west(state->direction)) ||
        (state->x == GAME_WIDTH - 1 && is_east(state->direction))) {
        // Apply logic for flipping direction when the ball hits a wall
        state->direction = deflect_wall(state->direction);
    }

    if (state->y == GAME_HEIGHT - 2) {
        // Logic for possible paddle hits
        if (state->x == paddle_position) {
            // When the ball is in the middle of the paddle, then bounce
            if (state->direction == SE) {
                state->direction = NE;
            } else if (state->direction == SW) {
                state->direction = NW;
            }
        } else if ((state->x == paddle_position - 1 ||
                    state->x == paddle_position - 2) &&
                   state->direction == SE) {
            // If the ball is on the left of the paddle, and traveling to the
            // right, then bounce
            state->direction = NE;
        } else if ((state->x == paddle_position + 1 ||
                    state->x == paddle_position + 2) &&
                   state->direction == SW) {
            // If the ball is on the right of the paddle, and traveling to the
            // left, then bounce
            state->direction = NW;
        }
    }

    // Update coordinates of the ball based on direction state
    state->x += (is_east(state->direction) ? 1 : -1);

    // Only apply transfer logic once x cordinate has been updated, so that the
    // ball appears in a different x coord on the other side
    if (state->y == 0 && is_north(state->direction)) {
        // If the ball is in the top row and moving up, then we indicate the
        // ball should transfer to the other player
        return Transfer;
    }

    state->y += (is_south(state->direction) ? 1 : -1);

    return Normal;
}

Direction mirror_direction(Direction direction)
{
    switch (direction) {
        case NE:
            return SW;
        case SE:
            return NW;
        case SW:
            return NE;
        case NW:
            return SE;
    }
}
