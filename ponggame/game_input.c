/*
 Input handling (navswitch & button).
 Authors: Josh Craythorne (usercode); Jamie Pedersen (usercode)
*/
#include "game_input.h"
#include "navswitch.h"
#include "button.h"
#include "game.h"

#define PADDLE_Y_MIN 1
#define PADDLE_Y_MAX (DISP_H - 2)

void game_input_service_always(game_state_t* s)
{
    /* Pause toggle via pushbutton always active. */
    if (button_push_event_p(BUTTON1)) {
        s->paused = !s->paused;
    }
}

void game_input_update(game_state_t* s)
{
    /* Difficulty adjust: EAST = faster; WEST = slower. */
    if (navswitch_push_event_p(NAVSWITCH_EAST)) {
        if (s->ball_step_ticks > BALL_STEP_TICKS_MIN)
            s->ball_step_ticks -= 10;
    }
    if (navswitch_push_event_p(NAVSWITCH_WEST)) {
        if (s->ball_step_ticks < BALL_STEP_TICKS_MAX)
            s->ball_step_ticks += 10;
    }

    /* Serve to start a rally: ONLY on navswitch PUSH edge. */
    if (s->waiting_to_serve && navswitch_push_event_p(NAVSWITCH_PUSH)) {
        s->ball.present = true;
        s->ball.x = 0;
        s->ball.y = clamp_i8(s->paddle.y, 0, (int8_t)DISP_H - 1);
        s->ball.vx = +1;
        s->ball.vy = 0;
        s->waiting_to_serve = false;
    }

    /* Paddle movement (hold-friendly). */
    s->paddle_step_accum++;
    bool move_now = (s->paddle_step_accum >= PADDLE_MOVE_TICKS)
                 || navswitch_push_event_p(NAVSWITCH_NORTH)
                 || navswitch_push_event_p(NAVSWITCH_SOUTH);

    if (move_now) {
        if (navswitch_down_p(NAVSWITCH_NORTH)) {
            s->paddle.y = clamp_i8(s->paddle.y - 1, PADDLE_Y_MIN, PADDLE_Y_MAX);
        }
        if (navswitch_down_p(NAVSWITCH_SOUTH)) {
            s->paddle.y = clamp_i8(s->paddle.y + 1, PADDLE_Y_MIN, PADDLE_Y_MAX);
        }
        s->paddle_step_accum = 0;
    }
}