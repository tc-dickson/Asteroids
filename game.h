#ifndef GAMECONTROL_H_
#define GAMECONTROL_H_

#include "asteroid.h"
#include <stdint.h>
#include "display.h"

// Call this before using any wamControl_ functions.
void game_init();

// Call this to set how much time is consumed by each tick of the controlling
// state machine. This information makes it possible to set the awake and sleep
// time of moles in ms, not ticks.
void game_setMsPerTick(uint16_t msPerTick);

// This returns the time consumed by each tick of the controlling state machine.
uint16_t game_getMsPerTick();

// Standard tick function.
void game_tick();

// Enable the state machine (interlock).
void game_enable();

// Disable the state machine (interlock).
void game_disable();

// Use this predicate to see if the game is finished.
bool game_isGameOver();

#endif /* GAMECONTROL_H_ */
