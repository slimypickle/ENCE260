# UCFK4 Pong — Two-Player, Real-Time, IR-Synchronized

**Authors:** Josh Craythorne, Jamie Pedersen  
**Date:** 2025-10-06  
**Student IDs** 41503887, 92133281

---

## Overview
A two-player Pong game built for the **UC FunKit (UCFK4)** microcontroller board.  
Both players run the **same binary** - each board displays half the arena.  
The ball travels between boards via **infrared (IR) communication**, creating a seamless, real-time rally.

---

##  Controls
| Control | Function |
|----------|-----------|
| **NAVSWITCH NORTH/SOUTH** | Move paddle up/down |
| **NAVSWITCH EAST/WEST** | Increase / decrease ball speed (difficulty) |
| **NAVSWITCH PUSH** | Serve to start a new rally |

---

##  Game Rules
- A **serve** starts a new rally from your left edge moving right.
- Return the ball by aligning your paddle at the right edge.
- If you **miss**, your opponent scores and the ball transfers to their board.
- The receiver automatically spawns the ball at their left edge.
- Display indicators:
  - `P` - Paused  
  - `W / L / D` - Waiting to serve (Win/Loss/Draw indicator)  
  - Otherwise - Paddle and active ball

---

## Tips
- Try hitting different parts of the paddle to aim shots.
- Use **EAST/WEST** to tweak difficulty by changing ball speed.
- Watch the **angle effects** during rallies - longer volleys can become tricky!

---

## Build & Flash
From your project directory (e.g., `assignment/group_123`) on an **Erskine Lab** machine:

```bash
make clean
make
make program

## AI Statement of Use

Artificial intelligence tools, including ChatGPT (OpenAI, 2025), were used **only** for assistance with:
- Code formatting, debugging suggestions, and syntax clarification.
- Improving clarity and presentation of existing code comments.

All logic, algorithms, and final implementations were written, tested, and verified by the authors.  
No AI-generated code was directly copied into the final program without full understanding and modification by the group.
