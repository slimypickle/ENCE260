/*
 IR message format and reliable transfer (BALL + ACK).
 Authors: Josh Craythorne (usercode); Jamie Pedersen (usercode)
*/
#ifndef GAME_IR_H
#define GAME_IR_H

#include "system.h"
#include "game_model.h"

/* Initialize/reset IR transfer state. */
void game_ir_init(void);

/* Service IR receive/transmit (poll each tick). */
void game_ir_service(game_state_t* s);

/* Queue a ball transfer to opponent (called on miss).
   fast_hint nudges remote speed: true=faster, false=slower. */
void game_ir_queue_ball_transfer(uint8_t y, int8_t vy, bool fast_hint);

#endif /* GAME_IR_H */