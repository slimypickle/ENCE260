/*
 Game state model (singleton) and initialization.
 Authors: Josh Craythorne (usercode); Jamie Pedersen (usercode)
*/
#ifndef GAME_MODEL_H
#define GAME_MODEL_H

#include "system.h"
#include "game.h"

/* Ball model. */
typedef struct
{
    int8_t  x;       /* 0..4 */
    int8_t  y;       /* 0..6 */
    int8_t  vx;      /* -1 or +1 (locally +1 moves right) */
    int8_t  vy;      /* -1, 0, +1 */
    bool    present; /* ball active on this board */
} ball_t;

/* Paddle model (right-edge paddle). */
typedef struct
{
    int8_t  y;   /* center y row */
    uint8_t len; /* half length: 1 => rows [y-1..y+1] */
} paddle_t;

/* Score model. */
typedef struct
{
    uint8_t my_score;
    uint8_t their_score;
} score_t;

/* Global game state. */
typedef struct
{
    ball_t     ball;
    paddle_t   paddle;
    score_t    score;

    /* Timing knobs */
    uint16_t   ball_step_ticks;
    uint16_t   ball_step_accum;

    uint16_t   paddle_step_accum;

    /* UI flags */
    bool       paused;
    bool       waiting_to_serve;
} game_state_t;

/* Singleton accessor. */
game_state_t* game_state(void);

/* Initialize/reset the whole game. */
void game_init(void);

#endif /* GAME_MODEL_H */