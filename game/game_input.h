/*
 Input handling (navswitch & button).
 Authors: Josh Craythorne (usercode); Jamie Pedersen (usercode)
*/
#ifndef GAME_INPUT_H
#define GAME_INPUT_H

#include "game_model.h"

/* Process inputs and update state (paddle move, pause, difficulty, serve). */
void game_input_update(game_state_t* s);

#endif /* GAME_INPUT_H */