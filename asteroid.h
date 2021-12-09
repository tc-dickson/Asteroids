#ifndef ASTEROID_H_
#define ASTEROID_H_

#include <stdbool.h>
#include <stdint.h>
#include <display.h>

// it adds an asteroid. What's there to explain?
struct Asteroid* asteroid_addAsteroid(int16_t myX, int16_t myY, int8_t myXVelocity, int8_t myYVelocity, uint8_t myRadius);

void asteroid_generateAsteroids(uint8_t num);

void asteroid_enable();

void asteroid_disable();

// when laser or ship is detected within asteroid radius, asteroid
// will depending on size split into two smaller asteroids or be destroyed.
// Or a UFO if we decide to include those, Tier 3 goal on our Kickstarter
void asteroid_collision(struct Asteroid *asteroid);

// starts asteroid state machine, it doesn't really have that much to do
void asteroid_init();

// standard tick function, capable of adding, drawing, moving, and destroying asteroids
// as dictated by control program
void asteroid_tick();

uint8_t asteroid_getCount();

struct Asteroid* asteroid_getHeadAsteroid();

struct Asteroid* asteroid_getTailAsteroid();

void asteroid_eraseAll();

#endif /* ASTEROID_H_ */
