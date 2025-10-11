# UCFK4 Pong — Two-Player, Real-Time, IR-Synchronized

Authors: Josh Craythorne, Jamie Pedersen
Date: 2025-10-06

## Overview
A two-player Pong game for the UC FunKit (UCFK4). Both boards run the same binary. One player serves; the ball travels across each board’s LED matrix and “exits” via IR to the other board, which continues the play.

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