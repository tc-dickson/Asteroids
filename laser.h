#ifndef LASER_H_
#define LASER_H_

#define LASER_VELOCITY_MAX 5

#include <display.h>
#include <stdbool.h>
#include <stdint.h>

// it adds an laser. What's there to explain?
struct Laser *laser_addLaser(int16_t myX, int16_t myY, int8_t myXVelocity,
                             int8_t myYVelocity);

void laser_enable();

void laser_disable();

void laser_collision(struct Laser *laser);

// starts laser state machine, it doesn't really have that much to do
void laser_init();

// standard tick function, capable of adding, drawing, moving, and destroying
// laser as dictated by control program
void laser_tick();

uint8_t laser_getCount();

struct Laser *laser_getHeadLaser();

struct Laser *laser_getTailLaser();

void laser_eraseAll();

#endif /* LASER_H_ */
