#ifndef SPACESHIP_H_
#define SPACESHIP_H_

#include <stdint.h>
#include <stdbool.h>

// Initialize the spaceship with starting values.
void spaceship_init();

// Use this function to test various parts of the spaceship code.
void spaceship_runTest(bool rotateCCW);

// Draw the spaceship.
void spaceship_drawShip(bool draw);

// Function to rotate the spaceship. If rotateCC is true then the spaceship will
// rotate counter-clockwise. Otherwise it will rotate clockwise.
void spaceship_rotateShip(bool rotateCCW);

#endif // SPACESHIP_H_