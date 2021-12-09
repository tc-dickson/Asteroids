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

// Function that handles the movement and firing of the ship.
void spaceship_moveShip(bool rotateCCW, bool rotateCW, bool moveForward, bool shoot);

// Populate an array of x, y coordinates of the spaceship's centerpoint and verticies.
void spaceship_getPrinciplePoints( coordinates_t *coordinatesArr);

#endif // SPACESHIP_H_