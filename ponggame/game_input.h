/*
 Input handling (navswitch & button).
 Authors: Josh Craythorne (usercode); Jamie Pedersen (usercode)
*/
#ifndef GAME_INPUT_H
#define GAME_INPUT_H

#include "game_model.h"

/* Process inputs that must always work (pause/resume). */
void game_input_service_always(game_state_t* s);

/* Process gameplay inputs (paddle, difficulty, serve) when not paused. */
void game_input_update(game_state_t* s);

#endif /* GAME_INPUT_H */