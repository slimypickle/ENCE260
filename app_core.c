// Josh Craythorne & Jamie Pedersen

#include "physics_ball.h"
#include "control_paddle.h"
#include "app_config.h"

#include "ir_uart.h"
#include "navswitch.h"
#include "system.h"
#include "pacer.h"
#include "tinygl.h"
#include "font3x5_1.h"

#include <stdbool.h>
#include <stdint.h>

/* ========================= Types & Globals ========================= */

typedef enum {
    CORE_STATE_HOME,
    CORE_STATE_SETTINGS,
    CORE_STATE_INGAME,
    CORE_STATE_OVER,
} core_state_t;

/* One-byte IR messages. */
typedef enum {
    CORE_MSG_START = 7,
    CORE_MSG_OPPONENT_MISS,
    CORE_MSG_TRANSFER,
} core_msg_t;

/* Difficulty labels & rates (unchanged). */
typedef enum {
    CORE_DIFF_LOW = 10,
    CORE_DIFF_MED,
    CORE_DIFF_HIGH,
} core_diff_t;

static const char* core_diff_label(core_diff_t d)
{
    switch (d) {
    case CORE_DIFF_LOW:  return "LO";
    case CORE_DIFF_MED:  return "ME";
    case CORE_DIFF_HIGH:
    default:             return "HI";
    }
}

static uint8_t core_diff_ball_rate(core_diff_t d)
{
    switch (d) {
    case CORE_DIFF_LOW:  return 2;
    case CORE_DIFF_MED:  return 7;
    case CORE_DIFF_HIGH:
    default:             return 10;
    }
}

/* Global state. */
static uint16_t     g_ball_ticks = 0;
static core_state_t g_state      = CORE_STATE_HOME;
static core_diff_t  g_diff       = CORE_DIFF_LOW;
static uint8_t      g_pad_x      = 1;  /* paddle center x (1..GAME_WIDTH-2) */

/* Ball state (owned locally when has_ball == true). */
static ball_state_t g_ball = {
    .dir      = BALL_DIR_SE,
    .x        = 0,
    .y        = 0,
    .has_ball = true,
};

/* Round result flag for OVER. */
static bool g_won = false;
static uint8_t g_score_me = 0;
static uint8_t g_score_op = 0;
/* Show scoreboard on HOME immediately after leaving OVER. */
static bool    g_show_scores = false;


static void task_display(void)
{
    static bool init = false;
    static core_state_t prev_state = (core_state_t)255;
    static bool prev_show_scores = false;

    if (!init) {
        tinygl_init(DISPLAY_TASK_RATE);
        tinygl_font_set(&font3x5_1);
        tinygl_text_speed_set(TEXT_RATE);
        tinygl_text_mode_set(TINYGL_TEXT_MODE_SCROLL);
        tinygl_text_dir_set(TINYGL_TEXT_DIR_ROTATE);
        tinygl_text("PRESS TO START :)");
        init = true;
        prev_state = g_state;
        prev_show_scores = g_show_scores;
    }

    /* Update content ONLY when state or scoreboard flag changes. */
    if (g_state != prev_state || g_show_scores != prev_show_scores) {
        switch (g_state) {
        case CORE_STATE_HOME:
            if (g_show_scores) {
                tinygl_text_mode_set(TINYGL_TEXT_MODE_STEP);
                char sbuf[6]; /* "x-y" */
                sbuf[0] = '0' + g_score_me;
                sbuf[1] = '-';
                sbuf[2] = '0' + g_score_op;
                sbuf[3] = '\0';
                tinygl_text(sbuf);
            } else {
                tinygl_text_mode_set(TINYGL_TEXT_MODE_SCROLL);
                tinygl_text_dir_set(TINYGL_TEXT_DIR_ROTATE);
                tinygl_text("WELCOME TO PONG! PRESS TO CONTINUE");
            }
            break;

        case CORE_STATE_SETTINGS:
            tinygl_text_mode_set(TINYGL_TEXT_MODE_STEP);
            tinygl_text(core_diff_label(g_diff));
            break;

        case CORE_STATE_INGAME:
            /* nothing to set; we draw pixels directly each frame */
            break;

        case CORE_STATE_OVER: {
            tinygl_text_mode_set(TINYGL_TEXT_MODE_STEP);
            char obuf[8]; /* "2-1 W" */
            obuf[0] = '0' + g_score_me;
            obuf[1] = '-';
            obuf[2] = '0' + g_score_op;
            obuf[3] = ' ';
            obuf[4] = g_won ? 'W' : 'L';
            obuf[5] = '\0';
            tinygl_text(obuf);
            } break;
        }
        prev_state = g_state;
        prev_show_scores = g_show_scores;
    }

    /* Per-frame drawing for in-game scene. */
    if (g_state == CORE_STATE_INGAME) {
        tinygl_clear();

        if (g_ball.has_ball) {
            tinygl_point_t p = { g_ball.y, (uint8_t)(TINYGL_HEIGHT - 1 - g_ball.x) };
            tinygl_draw_point(p, true);
        }

        tinygl_point_t a = { (uint8_t)(TINYGL_WIDTH - 1),
                             (uint8_t)(TINYGL_HEIGHT - g_pad_x) };
        tinygl_point_t b = { (uint8_t)(TINYGL_WIDTH - 1),
                             (uint8_t)(TINYGL_HEIGHT - g_pad_x - 2) };
        tinygl_draw_line(a, b, true);
    }

    tinygl_update();
}

/* ============================= Input ============================= */

static void task_input(void)
{
    static bool init = false;

    if (!init) {
        navswitch_init();
        init = true;
    }

    navswitch_update();

    switch (g_state) {
    case CORE_STATE_HOME:
        if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
            g_state = CORE_STATE_SETTINGS;
            g_show_scores = false;     /* hide scoreboard after leaving HOME */
            /* Force text update on state change */
            tinygl_text_mode_set(TINYGL_TEXT_MODE_STEP);
            tinygl_text(core_diff_label(g_diff));
        }
        break;

    case CORE_STATE_SETTINGS:
        if (navswitch_push_event_p(NAVSWITCH_EAST) && g_diff > CORE_DIFF_LOW) {
            g_diff--;
            /* Live update of difficulty text */
            tinygl_text_mode_set(TINYGL_TEXT_MODE_STEP);
            tinygl_text(core_diff_label(g_diff));
        } else if (navswitch_push_event_p(NAVSWITCH_WEST) && g_diff < CORE_DIFF_HIGH) {
            g_diff++;
            /* Live update of difficulty text */
            tinygl_text_mode_set(TINYGL_TEXT_MODE_STEP);
            tinygl_text(core_diff_label(g_diff));
        } else if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
            /* I initiate the game: I serve locally. */
            g_state = CORE_STATE_INGAME;

            /* Notify opponent with difficulty; they will auto-enter INGAME. */
            ir_uart_putc((uint8_t)CORE_MSG_START);
            ir_uart_putc((uint8_t)g_diff);

            /* Reset local round (NOT the scoreboard). */
            g_ball.x = 0;
            g_ball.y = 0;
            g_ball.dir = BALL_DIR_SE;
            g_ball.has_ball = true;  /* starter serves */
            g_ball_ticks = 0;
        }
        break;

    case CORE_STATE_INGAME:
        update_paddle_position(&g_pad_x);
        break;

    case CORE_STATE_OVER:
        if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
            /* Return to HOME and show scoreboard there. */
            g_state = CORE_STATE_HOME;
            g_show_scores = true;

            /* Lightweight reset for next round setup (scores are kept). */
            g_pad_x = 1;
            g_ball.x = 0;
            g_ball.y = 0;
            g_ball.dir = BALL_DIR_SE;
            g_ball.has_ball = true;
            g_ball_ticks = 0;

            /* Force HOME text to scoreboard next frame handled in task_display via flag */
        }
        break;
    }
}

/* ============================= IR Link ============================= */

static void task_uart(void)
{
    static bool init = false;

    if (!init) {
        ir_uart_init();
        init = true;
    }

    /* Drain any available IR bytes. */
    while (ir_uart_read_ready_p()) {
        uint8_t m = ir_uart_getc();

        if (m == (uint8_t)CORE_MSG_START) {
            /* Next byte is difficulty; adopt it and auto-enter INGAME. */
            while (!ir_uart_read_ready_p()) {}
            uint8_t rd = ir_uart_getc();

            if (rd == (uint8_t)CORE_DIFF_LOW ||
                rd == (uint8_t)CORE_DIFF_MED ||
                rd == (uint8_t)CORE_DIFF_HIGH) {
                g_diff = (core_diff_t)rd;
            }

            /* Opponent initiated: we immediately join the game as receiver. */
            g_state         = CORE_STATE_INGAME;
            g_ball.has_ball = false;    /* receiver does NOT serve */
            g_ball_ticks    = 0;
        }
        else if (m == (uint8_t)CORE_MSG_OPPONENT_MISS) {
            /* Opponent says they missed: we win this round. */
            g_score_me++;              /* bump our score */
            g_won           = true;
            g_state         = CORE_STATE_OVER;
            g_ball.has_ball = false;
            g_ball_ticks    = 0;
        }
        else if (m == (uint8_t)CORE_MSG_TRANSFER) {
            /* Expect two more bytes: crossing X and current direction. */
            while (!ir_uart_read_ready_p()) {}
            uint8_t rx = ir_uart_getc();
            while (!ir_uart_read_ready_p()) {}
            uint8_t rdir = ir_uart_getc();

            /* Take ownership of ball at the top, mirrored direction. */
            g_ball.has_ball = true;
            g_ball.x   = rx < GAME_WIDTH ? rx : (GAME_WIDTH - 1);
            g_ball.y   = 0;
            g_ball.dir = ball_mirror_dir((ball_dir_t)rdir);
            g_ball_ticks = 0;
            g_state = CORE_STATE_INGAME; /* ensure we are in-game */
        } else {
            /* Unknown byte: ignore. */
        }
    }
}

/* ============================= Physics & Rules ============================= */

static void task_physics(void)
{
    if (g_state != CORE_STATE_INGAME) {
        return;
    }

    g_ball_ticks++;
    if (g_ball_ticks >= (GAME_CYCLE_RATE / core_diff_ball_rate(g_diff))) {
        g_ball_ticks = 0;

        ball_update_t r = ball_update_state(&g_ball, g_pad_x);

        if (r == BALL_UPDATE_TRANSFER) {
            g_ball.has_ball = false;

            /* Send crossing X and current direction. */
            ir_uart_putc((uint8_t)CORE_MSG_TRANSFER);
            ir_uart_putc((uint8_t)g_ball.x);
            ir_uart_putc((uint8_t)g_ball.dir);

        } else if (r == BALL_UPDATE_MISS) {
            /* We missed: inform opponent, end round (show L). */
            g_score_op++;  /* bump opponent's score */
            ir_uart_putc((uint8_t)CORE_MSG_OPPONENT_MISS);

            g_won            = false;
            g_state          = CORE_STATE_OVER;
            g_ball.has_ball  = false;
            g_ball_ticks     = 0;
        }
    }
}

/* ============================= Main ============================= */

int main(void)
{
    system_init();

    /*
     * Use pacer to call each task at its own rate.
     * app_config.h provides *_TASK_RATE values. We drive the pacer faster
     * and down-divide to those task rates.
     */
    const uint16_t PACER_HZ = 1000;   /* >= max task rate */
    pacer_init(PACER_HZ);

    /* Integer dividers (assumes *_TASK_RATE divides PACER_HZ). */
    const uint16_t disp_div = (DISPLAY_TASK_RATE  ? (PACER_HZ / DISPLAY_TASK_RATE)  : 1);
    const uint16_t btn_div  = (BUTTON_TASK_RATE   ? (PACER_HZ / BUTTON_TASK_RATE)   : 1);
    const uint16_t phys_div = (GAME_CYCLE_RATE    ? (PACER_HZ / GAME_CYCLE_RATE)    : 1);
    const uint16_t uart_div = (UART_CYCLE_RATE    ? (PACER_HZ / UART_CYCLE_RATE)    : 1);

    uint16_t tick = 0;

    for (;;) {
        pacer_wait();
        tick++;

        if (tick % disp_div == 0)  task_display();
        if (tick % btn_div  == 0)  task_input();
        if (tick % phys_div == 0)  task_physics();
        if (tick % uart_div == 0)  task_uart();

        /* prevent tick overflow corner cases */
        if (tick == 0xFFFE) tick = 0;
    }
}

