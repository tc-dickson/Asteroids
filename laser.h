#ifndef LASER_H_
#define LASER_H_

#define LASER_VELOCITY_MAX 10

#include <display.h>
#include <stdbool.h>
#include <stdint.h>

struct Laser {
  int16_t x;
  int16_t y;
  int8_t xVelocity;
  int8_t yVelocity;
  uint8_t radius;
  bool collision;
  struct Laser *previousLaser;
  struct Laser *nextLaser;
  uint8_t lifeCounter;
};

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
