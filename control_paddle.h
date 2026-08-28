// Josh Craythorne & Jamie Pedersen


#ifndef CONTROL_PADDLE_H
#define CONTROL_PADDLE_H

#include <stdint.h>

/* Update paddle center X position (game coordinates), clamped to [1, GAME_WIDTH-2]. */
void update_paddle_position(uint8_t* paddle_position);

#endif
