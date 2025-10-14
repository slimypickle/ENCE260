#include "paddle.h"
#include "navswitch.h"

void update_paddle_position(uint8_t *paddle_position)
{
    if (navswitch_push_event_p(NAVSWITCH_NORTH) && (*paddle_position != 5)) {
        (*paddle_position)++;
    }
    if (navswitch_push_event_p(NAVSWITCH_SOUTH) && (*paddle_position != 1)) {
        (*paddle_position)--;
    }
}
