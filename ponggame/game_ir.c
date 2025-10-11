/*
 IR message format and reliable transfer (BALL + ACK).
 Authors: Josh Craythorne (usercode); Jamie Pedersen (usercode)
*/
#include "game_ir.h"
#include "ir_serial.h"
#include "game.h"

/* IR resend/ACK state. */
static uint16_t ir_resend_accum;
static uint16_t ir_post_rx_grace;
static bool     ir_have_unacked;
static uint8_t  ir_unacked_msg;
static uint16_t ir_ack_cooldown;

/* Encode a BALL message. */
static uint8_t ir_encode_ball(uint8_t y, int8_t vy, bool fast_hint)
{
    uint8_t vy_code = IR_VY_CODE_Z0;
    if (vy < 0) vy_code = IR_VY_CODE_N1;
    else if (vy > 0) vy_code = IR_VY_CODE_P1;

    uint8_t msg = IR_TYPE_BALL
                | ((y & 0x07u) << 3)
                | ((vy_code & 0x03u) << 1)
                | (fast_hint ? 1u : 0u);
    return msg;
}

/* Decode a BALL message. */
static bool ir_decode_ball(uint8_t msg, uint8_t* y, int8_t* vy, bool* fast_hint)
{
    if ((msg & IR_TYPE_MASK) != IR_TYPE_BALL) return false;
    *y = (msg >> 3) & 0x07u;
    uint8_t vy_code = (msg >> 1) & 0x03u;
    switch (vy_code) {
        case IR_VY_CODE_N1: *vy = -1; break;
        case IR_VY_CODE_P1: *vy = +1; break;
        case IR_VY_CODE_Z0: *vy = 0;  break;
        default:            *vy = 0;  break;
    }
    *fast_hint = (msg & 1u) != 0u;
    return true;
}

void game_ir_init(void)
{
    ir_resend_accum  = 0;
    ir_post_rx_grace = 0;
    ir_have_unacked  = false;
    ir_unacked_msg   = 0;
    ir_ack_cooldown  = 0;
}

void game_ir_queue_ball_transfer(uint8_t y, int8_t vy, bool fast_hint)
{
    ir_unacked_msg  = ir_encode_ball(y, vy, fast_hint);
    ir_have_unacked = true;
    ir_resend_accum = 0;
    ir_post_rx_grace = IR_POST_RX_GRACE_TICKS;
}

static void ir_handle_rx(game_state_t* s, uint8_t rx)
{
    /* Handle ACK. */
    if ((rx & IR_TYPE_MASK) == IR_TYPE_ACK) {
        ir_have_unacked  = false;
        ir_post_rx_grace = 0;
        return;
    }

    /* Handle BALL. */
    uint8_t y; int8_t vy; bool fast_hint;
    if (ir_decode_ball(rx, &y, &vy, &fast_hint)) {
        /* ACK (rate limited) to cut down echo storms. */
        if (ir_ack_cooldown == 0) {
            ir_serial_transmit(IR_TYPE_ACK);
            ir_ack_cooldown = IR_ACK_RESP_COOLDOWN;
        }

        /* Spawn inbound ball on this board. */
        s->ball.present = true;
        s->ball.x = 0;
        s->ball.y = clamp_i8((int8_t)y, 0, (int8_t)DISP_H - 1);
        s->ball.vx = +1;
        s->ball.vy = clamp_i8(vy, -1, +1);

        /* Nudge speed toward hint (clamped). */
        if (fast_hint) {
            if (s->ball_step_ticks > BALL_STEP_TICKS_MIN + 10)
                s->ball_step_ticks -= 10;
        } else {
            if (s->ball_step_ticks < BALL_STEP_TICKS_MAX - 10)
                s->ball_step_ticks += 10;
        }

        s->waiting_to_serve = false;
    }
}

void game_ir_service(game_state_t* s)
{
    if (ir_ack_cooldown > 0)
        ir_ack_cooldown--;

    /* RX poll. */
    uint8_t rx = 0;
    ir_serial_ret_t ret = ir_serial_receive(&rx);
    if (ret == IR_SERIAL_OK)
        ir_handle_rx(s, rx);

    /* TX resend. */
    if (ir_have_unacked || (ir_post_rx_grace > 0)) {
        if (ir_resend_accum == 0) {
            ir_serial_transmit(ir_unacked_msg);
        }
        ir_resend_accum++;
        if (ir_resend_accum >= IR_RESEND_PERIOD_TICKS)
            ir_resend_accum = 0;

        if (ir_post_rx_grace > 0)
            ir_post_rx_grace--;
        else if (!ir_have_unacked)
            ir_resend_accum = 0;
    }
}