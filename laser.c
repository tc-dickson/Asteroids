#ifndef LASER_H_
#define LASER_H_

#include "laser.h"
#include "display.h"
#include "time.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define INIT_ST_MSG "laser_init_st\n"
#define PLAY_ST_MSG "laser_play_st\n"
#define ERROR_ST_MSG "laser_error_st\n"

#define LASER_LIFE_COUNTER_MAX 20
#define MAX_LASER_COUNT 5
#define LASER_RADIUS 2
#define DISPLAY_MID_X DISPLAY_WIDTH / 2
#define DISPLAY_MID_Y DISPLAY_HEIGHT / 2

static struct Laser *headLaser;
static struct Laser *tailLaser;
static uint8_t laserCount;
static bool enabled;

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

static enum laserControl_st_t { init_st, play_st } currentState, nextState;

// it adds an laser. What's there to explain?
struct Laser *laser_addLaser(int16_t myX, int16_t myY, int8_t myXVelocity,
                             int8_t myYVelocity) {
  struct Laser *newLaser = (struct Laser *)malloc(sizeof(struct Laser));
  if (newLaser) {
    newLaser->x = myX;
    newLaser->y = myY;
    newLaser->xVelocity = myXVelocity;
    newLaser->yVelocity = myYVelocity;
    newLaser->radius = LASER_RADIUS;
    newLaser->collision = false;
    newLaser->nextLaser = NULL;
    newLaser->lifeCounter = 0;
  }
  if (laserCount) {
    tailLaser->nextLaser = newLaser;
    newLaser->previousLaser = tailLaser;
    tailLaser = newLaser;
  } else {
    headLaser = newLaser;
    tailLaser = newLaser;
    newLaser->previousLaser = NULL;
  }
  ++laserCount;
  return newLaser;
}

void laser_drawLaser(struct Laser *laser) {
  display_fillCircle(laser->x, laser->y, laser->radius, DISPLAY_WHITE);
}

void laser_eraseLaser(struct Laser *laser) {
  display_fillCircle(laser->x, laser->y, laser->radius, DISPLAY_BLACK);
}

void laser_destroyLaser(struct Laser *laser) {
  if (laser != NULL && laserCount > 0) {
    laser_eraseLaser(laser);
    struct Laser *previous = laser->previousLaser;
    struct Laser *next = laser->nextLaser;
    if (next != NULL) {
      next->previousLaser = previous;
    }
    if (previous != NULL) {
      previous->nextLaser = next;
    }
    if (laser == headLaser) {
      headLaser = next;
    }
    if (laser == tailLaser) {
      tailLaser = previous;
    }
    free(laser);
    --laserCount;
  }
}

void laser_eraseAll() {
  if (laserCount != 0) {
    struct Laser *laser = headLaser;
    while (laser != NULL) {
      struct Laser *temp = laser->nextLaser;
      laser_eraseLaser(laser);
      laser_destroyLaser(laser);
      laser = temp;
      --laserCount;
    }
  }
}

void laser_enable() {
  enabled = true;
  printf("we got this far\n");
}

void laser_disable() {
  laser_eraseAll();
  enabled = false;
  printf("now we're this far\n");
}

void laser_collision(struct Laser *laser) {
  laser->collision = true;
  laser_eraseLaser(laser);
}

void laser_moveLaser(struct Laser *laser) {
  if (laser->x <= (-1 * laser->radius)) {
    laser->x = DISPLAY_WIDTH;
  } else if (laser->x >= (DISPLAY_WIDTH + laser->radius)) {
    laser->x = 0;
  } else {
    laser->x = laser->x + laser->xVelocity;
  }
  if (laser->y <= (-1 * laser->radius)) {
    laser->y = DISPLAY_HEIGHT;
  } else if (laser->y >= (DISPLAY_HEIGHT + laser->radius)) {
    laser->y = 0;
  } else {
    laser->y = laser->y + laser->yVelocity;
  }
}

// starts laser state machine, it doesn't really have that much to do
void laser_init() {
  laserCount = 0;
  enabled = false;
}

void laser_debugState() {
  switch (currentState) {
  case init_st:
    printf(INIT_ST_MSG);
    break;
  case play_st:
    printf(PLAY_ST_MSG);
    break;
  default:
    printf(ERROR_ST_MSG);
    break;
  }
}

// standard tick function, capable of adding, drawing, moving, and destroying
// lasers
void laser_tick() {
  if (enabled) {
    laser_debugState();
  }
  switch (currentState) {
  case init_st:
    if (!enabled) {
      laserCount = 0;
      nextState = init_st;
    } else {
      nextState = play_st;
    }
    break;
  case play_st:
    if (!enabled) {
      laser_eraseAll();
      nextState = init_st;
    } else {
      struct Laser *laser = headLaser;
      while (laser != NULL && headLaser != NULL) {
        if (laser->lifeCounter >= LASER_LIFE_COUNTER_MAX) {
          laser_destroyLaser(laser);
        } else if (laser->collision) {
          laser_collision(laser);
        } else {
          laser_eraseLaser(laser);
          laser_moveLaser(laser);
          laser_drawLaser(laser);
        }
        laser->lifeCounter = laser->lifeCounter + 1;
        laser = laser->nextLaser;
      }
      nextState = play_st;
    }
    break;
  default:
    break;
  }
  currentState = nextState;
  // They haven't broken me yet, I'm not following the coding standard if it
  // means nothing in the end
}

uint8_t laser_getCount() { return laserCount; }

struct Laser *laser_getHeadLaser() {
  return headLaser;
}

struct Laser *laser_getTailLaser() {
  return tailLaser;
}
#endif /* LASER_H_ */
