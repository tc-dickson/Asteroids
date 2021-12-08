#ifndef SPACESHIP_H_
#define SPACESHIP_H_

#include <stdbool.h>
#include <stdint.h>

// Definitions for the positioning of the verticies of the spaceship.
#define NUM_VERTICIES 5

// Define a new type for the coordinate struct members.
typedef uint16_t coordMem_t;

// Define a struct to hold x, y, coordinates as integers for the LCD screen and collision detection.
typedef struct {
  coordMem_t x;
  coordMem_t y;
} coordinates_t;

// Initialize the spaceship with starting values.
void spaceship_init();

// Use this function to test various parts of the spaceship code.
void spaceship_runTest(bool rotateCCW, bool fireRockets);

// Draw the spaceship.
void spaceship_drawShip(bool draw);

// Function to rotate the spaceship. If rotateCC is true then the spaceship will
// rotate counter-clockwise. Otherwise it will rotate clockwise.
void spaceship_rotateShip(bool rotateCCW);

// Function to move the spaceship forward in the direction it is facing if the
// rocketsAreFiring parameter is true. The spaceship will continue to coast for
// a time after the rockets are turned off (with rocketsAreFiring parameter
// being false).
void spaceship_translateShip(bool rocketsAreFiring);

// Return a list of x, y coordinates of the spaceship's centerpoint and verticies.
coordinates_t* spaceship_getPrinciplePoints();

#endif // SPACESHIP_H_