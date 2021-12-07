#ifndef SPACESHIP_H_
#define SPACESHIP_H_

#include <stdbool.h>
#include <stdint.h>

// Initialize the spaceship with starting values.
void spaceship_init();

// Use this function to test various parts of the spaceship code.
void spaceship_runTest(bool rotateCCW);

// Draw the spaceship.
void spaceship_drawShip(bool draw);

// Function to rotate the spaceship. If rotateCC is true then the spaceship will
// rotate counter-clockwise. Otherwise it will rotate clockwise.
void spaceship_rotateShip(bool rotateCCW);

// Function to move the spaceship forward in the direction it is facing if the
// rocketsAreFiring parameter is true. The spaceship will continue to coast for
// a time after the rockets are turned off (with rocketsAreFiring parameter
// being false).
void spaceship_rockets(bool rocketsAreFiring);

#endif // SPACESHIP_H_