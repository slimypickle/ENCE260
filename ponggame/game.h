/*
 Shared game-wide constants and small helpers.
 Authors: Josh Craythorne (usercode); Jamie Pedersen (usercode)
*/

#ifndef GAME_H
#define GAME_H

#include "system.h"

/* Core timing. */
#define PACER_RATE               1000u
#define TINYGL_RATE              PACER_RATE
#define TINYGL_TEXT_SPEED        15u

/* Display geometry. */
#define DISP_W                   5u
#define DISP_H                   7u

/* Ball physics cadence (ticks between steps at PACER_RATE). */
#define BALL_STEP_TICKS_MIN      30u
#define BALL_STEP_TICKS_MAX      200u
#define BALL_STEP_TICKS_DEFAULT  80u

/* Paddle movement cadence (ticks per step while holding). */
#define PADDLE_MOVE_TICKS        60u

/* IR protocol (compact 8-bit messages). */
#define IR_TYPE_SHIFT            6
#define IR_TYPE_MASK             0xC0u
#define IR_TYPE_NONE             0x00u
#define IR_TYPE_BALL             0x40u /* 01 << 6 */
#define IR_TYPE_ACK              0x80u /* 10 << 6 */

/* IR ball vertical velocity codes. */
#define IR_VY_CODE_N1            0u
#define IR_VY_CODE_P1            1u
#define IR_VY_CODE_Z0            2u

/* IR resend/ack cadence. */
#define IR_RESEND_PERIOD_TICKS   120u   /* ~8.3 Hz resend */
#define IR_POST_RX_GRACE_TICKS   500u   /* keep sending ~0.5 s after RX */
#define IR_ACK_RESP_COOLDOWN     30u    /* short delay between ACKs */

/* Utility clamp. */
static inline int8_t clamp_i8(int8_t v, int8_t lo, int8_t hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

#endif /* GAME_H */