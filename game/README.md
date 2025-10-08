# UCFK4 Pong — Two-Player, Real-Time, IR-Synchronized

Authors: Josh Craythorne (usercode); Jamie Pedersen (usercode)  
Date: 2025-10-06

## Overview
A highly interactive two-player Pong game for the UC FunKit (UCFK4). Both boards run the same binary. One player serves; the ball travels across each board’s LED matrix and “exits” via IR to the other board, which continues the play. Robust IR uses periodic retransmission and ACKs.

## Controls
- NAVSWITCH NORTH/SOUTH: Move paddle up/down.
- NAVSWITCH EAST/WEST: Increase/decrease ball speed (difficulty).
- NAVSWITCH PUSH: Serve when waiting to start a rally.
- BUTTON1: Pause/Resume.

## Rules
- Serve starts a new rally. The ball begins at your left edge moving right.
- Return the ball at your right edge with your paddle.
- Miss at your right edge: the opponent scores and the ball transfers to their board.
- The receiver automatically spawns the inbound ball at their left edge.
- The display shows:
  - P when paused,
  - W/L/D when waiting to serve (relative to score lead),
  - Otherwise the ball and your paddle.

## Build and Program
From `assignment/group_XXX` on an Erskine Lab machine (avr-gcc 7.3.0):
```
make clean
make
make program
```

## Requirements and APIs Used
- Single binary; both boards programmed from the same machine.
- Uses repository APIs:
  - Input: `drivers/navswitch`, `drivers/button`
  - Display: `drivers/display`, `drivers/ledmat`, `utils/tinygl`, `fonts/font5x7_1`
  - IR comms: `drivers/ir_serial`, `drivers/ir` (+ `drivers/avr/timer0`, `drivers/avr/delay`, `drivers/avr/prescale`)
  - Timing: `utils/pacer`
  - System init: `drivers/avr/system`

## Reliability
- Sender repeats the BALL transfer byte periodically until an ACK is received (or after a grace period).
- Receiver responds with ACK (rate-limited) and spawns the inbound ball immediately.

## AI Statement of Use
We used GitHub Copilot to:
- Structure the game into clear modules and write warning-free C code compatible with the supplied APIs.
- Design the compact IR message format and ACK/retransmit logic aligned with `ir_serial`.
- Draft and refine this README.

All code was reviewed and tested against the repository’s APIs and the assignment rubric. Final commits are by the authors. Only our application sources, the Makefile, and this README are included; no API modules are duplicated in the repository.