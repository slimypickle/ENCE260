/*
 Rendering (TinyGL).
 Authors: Josh Craythorne (usercode); Jamie Pedersen (usercode)
*/
#ifndef GAME_RENDER_H
#define GAME_RENDER_H

#include "game_model.h"

/* Draw a frame based on current game state. */
void game_render_frame(const game_state_t* s);

#endif /* GAME_RENDER_H */