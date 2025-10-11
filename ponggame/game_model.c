/*
 Game state model implementation.
 Authors: Josh Craythorne (usercode); Jamie Pedersen (usercode)
*/
#include "game_model.h"

#define PADDLE_INIT_Y            3
#define PADDLE_HALF_LEN_DEFAULT  1
#define BALL_INIT_VX             (+1)
#define BALL_INIT_VY             (0)

static game_state_t g_state;

game_state_t* game_state(void)
{
    return &g_state;
}

void game_init(void)
{
    /* Scores start at 0. */
    g_state.score.my_score = 0;
    g_state.score.their_score = 0;

    /* Paddle. */
    g_state.paddle.y = PADDLE_INIT_Y;
    g_state.paddle.len = PADDLE_HALF_LEN_DEFAULT;

    /* Ready-to-serve ball on this board from the left edge. */
    g_state.ball.present = true;
    g_state.ball.x = 0;
    g_state.ball.y = 3;
    g_state.ball.vx = BALL_INIT_VX;
    g_state.ball.vy = BALL_INIT_VY;

    /* Timing. */
    g_state.ball_step_ticks = BALL_STEP_TICKS_DEFAULT;
    g_state.ball_step_accum = 0;
    g_state.paddle_step_accum = 0;

    /* Flags. */
    g_state.paused = false;
    g_state.waiting_to_serve = false; /* ball already present to serve */
}