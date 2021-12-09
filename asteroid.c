#ifndef ASTEROID_H_
#define ASTEROID_H_

#include "asteroid.h"
#include "display.h"
#include "time.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define INIT_ST_MSG "asteroid_init_st\n"
#define PLAY_ST_MSG "asteroid_play_st\n"
#define ERROR_ST_MSG "asteroid_error_st\n"

#define MAX_ASTEROID_COUNT 25
#define LARGE_ASTEROID_RADIUS 24
#define MEDIUM_ASTEROID_RADIUS 12
#define SMALL_ASTEROID_RADIUS 6
#define VELOCITY_VARIANCE 8
#define DISPLAY_MID_X DISPLAY_WIDTH / 2
#define DISPLAY_MID_Y DISPLAY_HEIGHT / 2

static struct Asteroid *headAsteroid;
static struct Asteroid *tailAsteroid;
static uint8_t asteroidCount;
static bool enabled;
static uint16_t counter;

struct Asteroid {
  int16_t x;
  int16_t y;
  int8_t xVelocity;
  int8_t yVelocity;
  uint8_t radius;
  bool collision;
  struct Asteroid *previousAsteroid;
  struct Asteroid *nextAsteroid;
};

static enum asteroidControl_st_t { init_st, play_st } currentState;

// it adds an asteroid. What's there to explain?
struct Asteroid *asteroid_addAsteroid(int16_t myX, int16_t myY,
                                      int8_t myXVelocity, int8_t myYVelocity,
                                      uint8_t myRadius) {
  struct Asteroid *newAsteroid =
      (struct Asteroid *)malloc(sizeof(struct Asteroid));

  if (newAsteroid) {
    newAsteroid->x = myX;
    newAsteroid->y = myY;
    newAsteroid->xVelocity = myXVelocity;
    newAsteroid->yVelocity = myYVelocity;
    newAsteroid->radius = myRadius;
    newAsteroid->collision = false;
    newAsteroid->nextAsteroid = NULL;
  }
  if (asteroidCount) {
    tailAsteroid->nextAsteroid = newAsteroid;
    newAsteroid->previousAsteroid = tailAsteroid;
    tailAsteroid = newAsteroid;
  } else {
    headAsteroid = newAsteroid;
    tailAsteroid = newAsteroid;
    newAsteroid->previousAsteroid = NULL;
  }
  ++asteroidCount;
  return newAsteroid;
}

void asteroid_generateAsteroids(uint8_t num) {
  srand(0);
  for (int i = 0; i < num; i++) {
    uint8_t xVel = 1 + rand() % (VELOCITY_VARIANCE / 2);
    if (rand() % 2) {
      xVel = -xVel;
    }
    uint8_t yVel = 1 + rand() % (VELOCITY_VARIANCE / 2);
    if (rand() % 2) {
      yVel = -yVel;
    }
    if (rand() % 2) {
      uint16_t xPos = rand() % DISPLAY_WIDTH;
      asteroid_addAsteroid(xPos, 0, xVel, yVel, LARGE_ASTEROID_RADIUS);
    } else {
      uint16_t yPos = rand() % DISPLAY_WIDTH;
      asteroid_addAsteroid(0, yPos, xVel, yVel, LARGE_ASTEROID_RADIUS);
    }
  }
}

void asteroid_drawAsteroid(struct Asteroid *asteroid) {
  display_drawCircle(asteroid->x, asteroid->y, asteroid->radius, DISPLAY_WHITE);
}

void asteroid_eraseAsteroid(struct Asteroid *asteroid) {
  display_drawCircle(asteroid->x, asteroid->y, asteroid->radius, DISPLAY_BLACK);
}

void asteroid_destroyAsteroid(struct Asteroid *asteroid) {
  if (asteroid && asteroidCount > 0) {
    asteroid_eraseAsteroid(asteroid);
    struct Asteroid *previous = asteroid->previousAsteroid;
    struct Asteroid *next = asteroid->nextAsteroid;
    if (next != NULL) {
      next->previousAsteroid = previous;
    }
    if (previous != NULL) {
      previous->nextAsteroid = next;
    }
    if (asteroid == headAsteroid) {
      headAsteroid = next;
    }
    if (asteroid == tailAsteroid) {
      tailAsteroid = previous;
    }
    free(asteroid);
    --asteroidCount;
  }
}

void asteroid_eraseAll() {
  if (asteroidCount != 0) {
    struct Asteroid *asteroid = headAsteroid;
    while (asteroid != NULL) {
      struct Asteroid *temp = asteroid->nextAsteroid;
      asteroid_eraseAsteroid(asteroid);
      asteroid_destroyAsteroid(asteroid);
      asteroid = temp;
      --asteroidCount;
    }
  }
}

void asteroid_enable() {
  enabled = true;
  printf("we got this far\n");
}

void asteroid_disable() {
  asteroid_eraseAll();
  enabled = false;
  printf("now we're this far\n");
}

// when laser or ship is detected within asteroid radius, asteroid
// will depending on size split into two smaller asteroids or be destroyed
void asteroid_collision(struct Asteroid *asteroid) {
  srand(time(0));
  asteroid->collision = true;
  asteroid_eraseAsteroid(asteroid);
  if (asteroid->radius >= LARGE_ASTEROID_RADIUS) {
    asteroid_addAsteroid(asteroid->x, asteroid->y,
                         asteroid->xVelocity + (rand() % VELOCITY_VARIANCE -
                                                VELOCITY_VARIANCE / 2),
                         asteroid->yVelocity + (rand() % VELOCITY_VARIANCE -
                                                VELOCITY_VARIANCE / 2),
                         MEDIUM_ASTEROID_RADIUS);
    asteroid_addAsteroid(asteroid->x, asteroid->y,
                         asteroid->xVelocity + (rand() % VELOCITY_VARIANCE -
                                                VELOCITY_VARIANCE / 2),
                         asteroid->yVelocity + (rand() % VELOCITY_VARIANCE -
                                                VELOCITY_VARIANCE / 2),
                         MEDIUM_ASTEROID_RADIUS);
    asteroid_destroyAsteroid(asteroid);
  } else if (asteroid->radius >= MEDIUM_ASTEROID_RADIUS) {
    asteroid_addAsteroid(asteroid->x, asteroid->y,
                         asteroid->xVelocity + (rand() % VELOCITY_VARIANCE -
                                                VELOCITY_VARIANCE / 2),
                         asteroid->yVelocity + (rand() % VELOCITY_VARIANCE -
                                                VELOCITY_VARIANCE / 2),
                         SMALL_ASTEROID_RADIUS);
    asteroid_addAsteroid(asteroid->x, asteroid->y,
                         asteroid->xVelocity + (rand() % VELOCITY_VARIANCE -
                                                VELOCITY_VARIANCE / 2),
                         asteroid->yVelocity + (rand() % VELOCITY_VARIANCE -
                                                VELOCITY_VARIANCE / 2),
                         SMALL_ASTEROID_RADIUS);
    asteroid_destroyAsteroid(asteroid);
  } else {
    asteroid_destroyAsteroid(asteroid);
  }
}

void asteroid_moveAsteroid(struct Asteroid *asteroid) {
  if (asteroid->x <= (-1 * asteroid->radius)) {
    asteroid->x = DISPLAY_WIDTH;
  } else if (asteroid->x >= (DISPLAY_WIDTH + asteroid->radius)) {
    asteroid->x = 0;
  } else {
    asteroid->x = asteroid->x + asteroid->xVelocity;
  }
  if (asteroid->y <= (-1 * asteroid->radius)) {
    asteroid->y = DISPLAY_HEIGHT;
  } else if (asteroid->y >= (DISPLAY_HEIGHT + asteroid->radius)) {
    asteroid->y = 0;
  } else {
    asteroid->y = asteroid->y + asteroid->yVelocity;
  }
}

// starts asteroid state machine, it doesn't really have that much to do
void asteroid_init() {
  asteroidCount = 0;
  enabled = false;
}

void asteroid_debugState() {
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

void asteroid_testProgram(uint16_t *counter) {
  if (*counter == 0) {
    // struct Asteroid *asteroid = asteroid_addAsteroid(DISPLAY_MID_X,
    // DISPLAY_MID_Y, 3, 3, 24);
  } else if (*counter >= 20) {
    headAsteroid->collision = true;
    *counter = 1;
  }
}

// standard tick function, capable of adding, drawing, moving, and destroying
// asteroids
void asteroid_tick() {
  if (enabled) {
    asteroid_debugState();
  }
  switch (currentState) {
  case init_st:
    if (!enabled) {
      asteroidCount = 0;
      currentState = init_st;
    } else {
      counter = 0;
      currentState = play_st;
    }
    break;
  case play_st:
    if (!enabled) {
      asteroid_eraseAll();
      currentState = init_st;
    } else {
      asteroid_testProgram(&counter);
      struct Asteroid *asteroid = headAsteroid;
      while (asteroid != NULL && headAsteroid != NULL) {
        if (asteroid->collision) {
          asteroid_collision(asteroid);
        } else {
          asteroid_eraseAsteroid(asteroid);
          asteroid_moveAsteroid(asteroid);
          asteroid_drawAsteroid(asteroid);
        }
        asteroid = asteroid->nextAsteroid;
      }
      counter++;
      currentState = play_st;
    }
    break;
  default:
    break;
  }
  // They haven't broken me yet, I'm not following the coding standard if it
  // means nothing in the end
}

uint8_t asteroid_getCount() { return asteroidCount; }

struct Asteroid *asteroid_getHeadAsteroid() {
  return headAsteroid;
}

struct Asteroid *asteroid_getTailAsteroid() {
  return tailAsteroid;
}
#endif /* ASTEROID_H_ */
