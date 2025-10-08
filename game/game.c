/*
 UCFK4 Pong — Two-Player, Real-Time, IR-Synchronized
 Authors: Josh Craythorne (usercode); Jamie Pedersen (usercode)
 Date: 2025-10-06
*/
#include "system.h"
#include "pacer.h"
#include "navswitch.h"
#include "button.h"
#include "tinygl.h"
#include "font.h"
#include "font5x7_1.h"
#include "ir_serial.h"

#include "game.h"
#include "game_model.h"
#include "game_input.h"
#include "game_physics.h"
#include "game_ir.h"
#include "game_render.h"

int main(void)
{
    system_init();
    pacer_init(PACER_RATE);

    navswitch_init();
    button_init();

    tinygl_init(TINYGL_RATE);
    tinygl_font_set(&font5x7_1);
    tinygl_text_speed_set(TINYGL_TEXT_SPEED);
    tinygl_text_mode_set(TINYGL_TEXT_MODE_STEP);

    ir_serial_init();
    game_ir_init();
    game_init();

    game_state_t* s = game_state();

    while (1) {
        pacer_wait();

        tinygl_update();
        navswitch_update();
        button_update();

        /* IR service must run even when paused to keep link robust. */
        game_ir_service(s);

        if (!s->paused) {
            game_input_update(s);
            game_physics_step(s);
        }

        game_render_frame(s);
    }
}