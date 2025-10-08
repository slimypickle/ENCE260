/*
 Ball stepping and collisions + scoring and IR handoff.
 Authors: Josh Craythorne (usercode); Jamie Pedersen (usercode)
*/
#include "game_physics.h"
#include "game.h"
#include "navswitch.h"
#include "game_ir.h"

static bool paddle_covers_y(const paddle_t* p, int8_t y)
{
    return (y >= (p->y - (int8_t)p->len)) && (y <= (p->y + (int8_t)p->len));
}

void game_physics_step(game_state_t* s)
{
    if (!s->ball.present)
        return;

    /* Step only when cadence elapsed. */
    s->ball_step_accum++;
    if (s->ball_step_accum < s->ball_step_ticks)
        return;
    s->ball_step_accum = 0;

    /* Vertical move + bounce on top/bottom. */
    int8_t ny = s->ball.y + s->ball.vy;
    if (ny < 0) {
        ny = 1; s->ball.vy = +1;
    } else if (ny >= (int8_t)DISP_H) {
        ny = (int8_t)DISP_H - 2; s->ball.vy = -1;
    }
    s->ball.y = ny;

    /* Horizontal move. */
    int8_t nx = s->ball.x + s->ball.vx;

    /* Right edge (our paddle at column DISP_W-1). */
    if (nx >= (int8_t)(DISP_W - 1)) {
        if (paddle_covers_y(&s->paddle, s->ball.y)) {
            /* Return to left and optionally add spin. */
            s->ball.vx = -1;
            if (navswitch_down_p(NAVSWITCH_EAST))
                s->ball.vy = clamp_i8(s->ball.vy + 1, -1, +1);
            if (navswitch_down_p(NAVSWITCH_WEST))
                s->ball.vy = clamp_i8(s->ball.vy - 1, -1, +1);
            nx = (int8_t)DISP_W - 2;
        } else {
            /* Miss: opponent scores, transfer ball to remote, remove locally. */
            s->score.their_score++;
            uint8_t y = (uint8_t)clamp_i8(s->ball.y, 0, (int8_t)DISP_H - 1);
            bool fast_hint = (s->ball_step_ticks <= (BALL_STEP_TICKS_DEFAULT - 10));
            game_ir_queue_ball_transfer(y, s->ball.vy, fast_hint);
            s->ball.present = false;
            return;
        }
    }

    /* Clamp left bound (we do not have a left paddle locally). */
    if (nx < 0) nx = 0;

    s->ball.x = nx;
}