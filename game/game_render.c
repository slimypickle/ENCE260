/*
 Rendering (TinyGL).
 Authors: Josh Craythorne (usercode); Jamie Pedersen (usercode)
*/
#include "game_render.h"
#include "tinygl.h"
#include "game.h"
#include "font5x7_1.h"

static inline void draw_pixel(uint8_t x, uint8_t y)
{
    if (x < DISP_W && y < DISP_H)
        tinygl_draw_point(tinygl_point(x, y), 1);
}

static void draw_paddle(const paddle_t* p, uint8_t col)
{
    for (int8_t r = -((int8_t)p->len); r <= (int8_t)p->len; r++) {
        int8_t y = p->y + r;
        if (y >= 0 && y < (int8_t)DISP_H)
            draw_pixel(col, (uint8_t)y);
    }
}

static void draw_ball(const ball_t* b)
{
    if (b->present && b->x >= 0 && b->x < (int8_t)DISP_W &&
        b->y >= 0 && b->y < (int8_t)DISP_H) {
        draw_pixel((uint8_t)b->x, (uint8_t)b->y);
    }
}

void game_render_frame(const game_state_t* s)
{
    tinygl_clear();

    /* Optional center "net". */
    draw_pixel(2, 3);

    /* Paddle at right edge. */
    draw_paddle(&s->paddle, DISP_W - 1);

    /* Ball. */
    draw_ball(&s->ball);

    /* Status overlays. */
    if (s->paused) {
        tinygl_draw_char('P', tinygl_point(0, 0));
    } else if (s->waiting_to_serve && !s->ball.present) {
        if (s->score.my_score > s->score.their_score)
            tinygl_draw_char('W', tinygl_point(0, 0));
        else if (s->score.my_score < s->score.their_score)
            tinygl_draw_char('L', tinygl_point(0, 0));
        else
            tinygl_draw_char('D', tinygl_point(0, 0));
    }
}