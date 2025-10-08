/*
 Ball stepping and collisions (paddle, walls) + scoring and handoff.
 Authors: Josh Craythorne (usercode); Jamie Pedersen (usercode)
*/
#ifndef GAME_PHYSICS_H
#define GAME_PHYSICS_H

#include "game_model.h"

/* Advance the ball according to cadence; handle collisions and scoring.
   On miss, queues an IR ball transfer for the opponent. */
void game_physics_step(game_state_t* s);

#endif /* GAME_PHYSICS_H */