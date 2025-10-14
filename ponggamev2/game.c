/*
Put your names, usernames, dates, description of the file etc here
*/

#include "../fonts/font3x5_1.h"
#include "ball.h"
#include "ir_uart.h"
#include "navswitch.h"
#include "paddle.h"
#include "system.h"
#include "task.h"
#include "tinygl.h"
#include <stdbool.h>

static uint16_t game_ticks = 0;

typedef enum {
    Home,
    Settings,
    InGame,
} GameState;

typedef enum : uint8_t { StartGame = 7, OpponentMiss, TransferBall } Message;

typedef enum : uint8_t {
    Low = 10,
    Medium,
    High,
} Difficulty;

char *difficulty_label(Difficulty difficulty)
{
    switch (difficulty) {
        case Low:
            return "LO";
        case Medium:
            return "ME";
        case High:
        default:
            return "HI";
    }
}

uint8_t difficulty_ball_rate(Difficulty difficulty)
{
    switch (difficulty) {
        case Low:
            return 1;
        case Medium:
            return 2;
        case High:
        default:
            return 3;
    }
}

static GameState game_state = Home;
static Difficulty difficulty = Low;

static BallState ball_state = {
    .direction = SE,
    .x = 0,
    .y = 0,
    .has_ball = true,
};

static uint8_t paddle_position = 1;

#define TEXT_RATE 20
#define DISPLAY_TASK_RATE 500

/**
 * Updates the LED matrix display during standard play using tinygl
 */
static void display_task(__unused__ void *data)
{
    static bool init = false;

    if (!init) {
        tinygl_init(DISPLAY_TASK_RATE);

        tinygl_font_set(&font3x5_1);
        tinygl_text_speed_set(TEXT_RATE);
        tinygl_text_mode_set(TINYGL_TEXT_MODE_SCROLL);
        tinygl_text_dir_set(TINYGL_TEXT_DIR_ROTATE);
        tinygl_text("WELCOME TO PONG! PRESS TO CONTINUE");

        init = true;
    }

    switch (game_state) {
        case Home:
            break;
        case Settings:
            tinygl_text_mode_set(TINYGL_TEXT_MODE_STEP);
            tinygl_text(difficulty_label(difficulty));
            break;
        case InGame:
            tinygl_clear();

            if (ball_state.has_ball) {
                tinygl_point_t ball_position = {
                    ball_state.y, TINYGL_HEIGHT - 1 - ball_state.x};

                tinygl_draw_point(ball_position, true);
            }

            tinygl_point_t start = {TINYGL_WIDTH - 1,
                                    TINYGL_HEIGHT - paddle_position};

            tinygl_point_t end = {TINYGL_WIDTH - 1,
                                  TINYGL_HEIGHT - paddle_position - 2};

            tinygl_draw_line(start, end, true);

            break;
    }

    tinygl_update();
}

#define BUTTON_TASK_RATE 100

/**
 * Handles logic related to navswitch control, pregame, or ingame
 */
void button_task(__unused__ void *data)
{
    static bool init = false;

    if (!init) {
        navswitch_init();

        init = true;
    }

    navswitch_update();

    switch (game_state) {
        case Home:
            if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
                game_state = Settings;
            }
            break;
        case Settings:
            if (navswitch_push_event_p(NAVSWITCH_EAST) && difficulty > Low) {
                difficulty--;
            } else if (navswitch_push_event_p(NAVSWITCH_WEST) &&
                       difficulty < High) {
                difficulty++;
            } else if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
                game_state = InGame;

                // Indicate to the other player the game has started on this
                // players side, and send the selected game difficulty

                ir_uart_putc(StartGame);
                ir_uart_putc(difficulty);
            }
            break;
        case InGame:
            update_paddle_position(&paddle_position);
            break;
    }
}

#define GAME_CYCLE_RATE 300

/**
 * Handles logic related to general gameplay behavior such as ball physics and
 * win/lose detection
 */
void game_cycle_task(__unused__ void *data)
{
    if (game_state != InGame) {
        return;
    }

    game_ticks++;
    if (game_ticks >= (GAME_CYCLE_RATE / difficulty_ball_rate(difficulty))) {
        game_ticks = 0;

        BallUpdateResult update_result =
            update_ball_state(&ball_state, paddle_position);

        if (update_result == Transfer) {
            ball_state.has_ball = false;

            // Send the TransferBall message as well as the position at which
            // the ball crossed (will be the new x position in the current
            // direction), and the current travel direction

            ir_uart_putc(TransferBall);
            ir_uart_putc(ball_state.x);
            ir_uart_putc(ball_state.direction);
        } else if (update_result == Miss) {
            // When the player misses, tell the opposing player this player
            // missed, and reset the ball and continue
            ir_uart_putc(OpponentMiss);

            ball_state.x = 0;
            ball_state.y = 0;
            ball_state.direction = SE;
        }
    }
}

#define UART_CYCLE_RATE 300

/**
 * Handles logic related to the receiving of messages from the other player
 */
void ir_uart_task(__unused__ void *data)
{
    static bool init = false;

    if (!init) {
        ir_uart_init();
        init = true;
    }

    if (ir_uart_read_ready_p()) {
        char message = ir_uart_getc();

        switch (message) {
            case StartGame:
                // Other user has started a game with a given difficulty, this
                // will be the next message

                Difficulty selected_difficulty = ir_uart_getc();

                difficulty = selected_difficulty;
                game_state = InGame;
                ball_state.has_ball = false;

                break;
            case TransferBall:
                // Other user's ball has reached boundary

                uint8_t position = ir_uart_getc();
                Direction prev_direction = ir_uart_getc();

                ball_state.x = TINYGL_HEIGHT - 1 - position;
                ball_state.y = 0;
                ball_state.has_ball = true;
                ball_state.direction = mirror_direction(prev_direction);

                // Reset the game ticks to zero so there is always a ball cycle
                // wait period when receiving the ball

                game_ticks = 0;

                break;
            case OpponentMiss:
                // Add score if we do scoring
                break;
            default:
                // Unknown message, do nothing
                break;
        }
    }
}

int main(void)
{
    task_t tasks[] = {{.func = display_task,
                       .period = TASK_RATE / DISPLAY_TASK_RATE,
                       .data = 0},
                      {.func = button_task,
                       .period = TASK_RATE / BUTTON_TASK_RATE,
                       .data = 0},
                      {.func = game_cycle_task,
                       .period = TASK_RATE / GAME_CYCLE_RATE,
                       .data = 0},
                      {
                          .func = ir_uart_task,
                          .period = TASK_RATE / UART_CYCLE_RATE,
                          .data = 0,
                      }};

    system_init();

    task_schedule(tasks, ARRAY_SIZE(tasks));

    return 0;
}
