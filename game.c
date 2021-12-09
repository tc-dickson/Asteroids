#include "game.h"
#include "asteroid.h"
#include "display.h"
#include "laser.h"
#include "spaceship.h"

#include <buttons.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// include button masks for ship controls
#define DISPLAY_MID_X DISPLAY_WIDTH / 2
#define DISPLAY_MID_Y DISPLAY_HEIGHT / 2
#define TITLE_TEXT "ASTEROIDS"
#define TITLE_TEXT_SIZE 5
#define TITLE_TEXT_X DISPLAY_MID_X - TITLE_TEXT_SIZE * 5 * 5.2
#define TITLE_TEXT_Y DISPLAY_MID_Y - TITLE_TEXT_SIZE * 7 * 1.5
#define TOUCH_TEXT "TOUCH TO PLAY"
#define TOUCH_TEXT_SIZE 3
#define TOUCH_TEXT_X DISPLAY_MID_X - TOUCH_TEXT_SIZE * 5 * 7.7
#define TOUCH_TEXT_Y DISPLAY_MID_Y + TOUCH_TEXT_SIZE * 5
#define GAME_OVER_TEXT "GAME OVER"
#define GAME_OVER_SIZE 4
#define GAME_OVER_TEXT_X DISPLAY_MID_X - TITLE_TEXT_SIZE * 5 * 4.2
#define GAME_OVER_TEXT_Y DISPLAY_MID_Y - TITLE_TEXT_SIZE * 7 * 1.5
#define PLAY_AGAIN_TEXT "TOUCH TO PLAY AGAIN"
#define PLAY_AGAIN_TEXT_SIZE 2
#define PLAY_AGAIN_TEXT_X DISPLAY_MID_X - TOUCH_TEXT_SIZE * 5 * 15 / 2
#define PLAY_AGAIN_TEXT_Y DISPLAY_MID_Y + TOUCH_TEXT_SIZE * 5
#define START_LIVES 3;
#define MAX_LIVES 5
#define LIFE_CHAR "A"
#define SCORE_TEXT_SIZE 2
#define SCORE_TEXT_X SCORE_TEXT_SIZE
#define SCORE_TEXT_Y SCORE_TEXT_SIZE
#define LIVES_X SCORE_TEXT_SIZE
#define LIVES_Y SCORE_TEXT_SIZE * 10
#define LIVES_SIZE SCORE_TEXT_SIZE
#define MAX_SCORE_SIZE 10

#define INIT_ST_MSG "game_init_st\n"
#define WELCOME_ST_MSG "game_welcome_st\n"
#define WELCOME_ADC_ST_MSG "game_welcome_adc_st\n"
#define PLAY_ST_MSG "game_play_st\n"
#define NEXT_LEVEL_ST_MSG "game_next_level_st\n"
#define DEATH_ST_MSG "game_death_st\n"
#define GAME_OVER_ST_MSG "game_over_st\n"
#define PLAY_AGAIN_ST_MSG "game_play_again_st\n"
#define PLAY_AGAIN_ADC_ST_MSG "game_play_again_adc_st\n"
#define ERROR_ST_MSG "game_error_st\n"
#define NUM_CHECK_POINTS (NUM_VERTICIES + 1)
#define ASTEROID_SCORE_POINTS 100

#define CONFIG_TIMER_PERIOD .1
#define ADC_COUNTER_MAX 1
#define REFRESH_COUNTER_MAX 2
#define DEATH_COUNTER_MAX 2 / CONFIG_TIMER_PERIOD
#define NEXT_LEVEL_COUNTER_MAX 2 / CONFIG_TIMER_PERIOD
#define GAME_OVER_COUNTER_MAX 2 / CONFIG_TIMER_PERIOD
#define PLAY_AGAIN_COUNTER_MAX 2 / CONFIG_TIMER_PERIOD

#define FIRE_BTN_NUM 3
#define RIGHT_BTN_NUM 2
#define THRUST_BTN_NUM 1
#define LEFT_BTN_NUM 0
#define LEFT_BTN0_MASK 0x1
#define THRUST_BTN1_MASK 0x2
#define RIGHT_BTN2_MASK 0x4
#define FIRE_BTN3_MASK 0x8

#define SQUARE_TERMS(A) ((A) * (A))

static uint16_t adcCounter;
static uint16_t deathCounter;
static uint16_t nextLevelCounter;
static uint16_t gameOverCounter;
static uint16_t playAgainCounter;
static uint16_t refreshCounter;
static uint8_t level;
static uint8_t lives = 0;
static uint16_t score = 0;
static uint16_t msPerTick;
static bool enabled;

static enum game_st_t {
  init_st,
  welcome_st,
  welcome_adc_st,
  play_st,
  next_level_st,
  death_st,
  game_over_st,
  play_again_st,
  play_again_adc_st
} currentState,
    nextState;

void game_drawWelcome(bool draw) {
  if (draw) {
    display_setTextColor(DISPLAY_WHITE);
  } else {
    display_setTextColor(DISPLAY_BLACK);
  }
  display_setCursor(TITLE_TEXT_X, TITLE_TEXT_Y);
  display_setTextSize(TITLE_TEXT_SIZE);
  display_print(TITLE_TEXT);
  display_setCursor(TOUCH_TEXT_X, TOUCH_TEXT_Y);
  display_setTextSize(TOUCH_TEXT_SIZE);
  display_print(TOUCH_TEXT);
}

void game_drawScore(bool draw) {
  if (draw) {
    display_setTextColor(DISPLAY_WHITE);
  } else {
    display_setTextColor(DISPLAY_BLACK);
  }
  display_setCursor(SCORE_TEXT_X, SCORE_TEXT_Y);
  display_setTextSize(SCORE_TEXT_SIZE);
  char scoreStr[MAX_SCORE_SIZE];
  sprintf(scoreStr, "%d", score);
  display_print(scoreStr);
}

void game_drawLives(bool draw) {
  if (draw) {
    display_setTextColor(DISPLAY_WHITE);
  } else {
    display_setTextColor(DISPLAY_BLACK);
  }
  display_setCursor(LIVES_X, LIVES_Y);
  display_setTextSize(LIVES_SIZE);
  char str[MAX_LIVES + 1];
  for (int i = 0; i <= lives; ++i) {
    if (i == (lives)) {
      str[i] = '\0';
    } else {
      str[i] = 'A';
    }
  }
  display_print(str);
}

void game_drawGameOver(bool draw) {
  if (draw) {
    display_setTextColor(DISPLAY_WHITE);
  } else {
    display_setTextColor(DISPLAY_BLACK);
  }
  display_setCursor(GAME_OVER_TEXT_X, GAME_OVER_TEXT_Y);
  display_setTextSize(GAME_OVER_SIZE);
  display_print(GAME_OVER_TEXT);
}

void game_drawPlayAgain(bool draw) {
  if (draw) {
    display_setTextColor(DISPLAY_WHITE);
  } else {
    display_setTextColor(DISPLAY_BLACK);
  }
  display_setCursor(PLAY_AGAIN_TEXT_X, PLAY_AGAIN_TEXT_Y);
  display_setTextSize(PLAY_AGAIN_TEXT_SIZE);
  display_print(PLAY_AGAIN_TEXT);
}

void game_incrementScore(uint16_t points) {
  game_drawScore(false);
  score = score + points;
  game_drawScore(true);
}

void game_changeLives(bool loseLife) {
  game_drawLives(false);
  if (loseLife) {
    lives--;
  } else {
    lives++;
  }
  game_drawLives(true);
}

void game_init() {
  enabled = false;
  level = 1;
  score = 0;
  refreshCounter = 0;
  lives = START_LIVES;
}

void game_setMsPerTick(uint16_t myMsPerTick) { msPerTick = myMsPerTick; }

// This returns the time consumed by each tick of the controlling state machine.
uint16_t game_getMsPerTick() { return msPerTick; }

void game_shipControl() {}

void game_checkLaserCollision() {

  struct Asteroid *asteroid = asteroid_getHeadAsteroid();
  while (asteroid != NULL) {
    struct Laser *laser = laser_getHeadLaser();
    while (laser != NULL) {
      if ((SQUARE_TERMS(asteroid->x - laser->x) <
           SQUARE_TERMS(asteroid->radius)) &&
          (SQUARE_TERMS(asteroid->y - laser->y) <
           SQUARE_TERMS(asteroid->radius))) {
        asteroid->collision = true;
        game_incrementScore(ASTEROID_SCORE_POINTS);
        printf("laser collision\n");
      }
      laser = laser->nextLaser;
    }
    asteroid = asteroid->nextAsteroid;
  }
}

bool game_checkShipCollision() {
  coordinates_t principlePoints[NUM_CHECK_POINTS];
  spaceship_getPrinciplePoints(principlePoints);
  for (int i = 0; i < NUM_CHECK_POINTS; ++i) {
    coordinates_t coordinate = principlePoints[i];
    struct Asteroid *asteroid = asteroid_getHeadAsteroid();
    while (asteroid != NULL) {
      if ((SQUARE_TERMS(asteroid->x - coordinate.x) <
           SQUARE_TERMS(asteroid->radius)) &&
          (SQUARE_TERMS(asteroid->y - coordinate.y) <
           SQUARE_TERMS(asteroid->radius))) {
        asteroid->collision = true;
        game_changeLives(true);
        printf("ship collision\n");
        return true;
      }
      asteroid = asteroid->nextAsteroid;
    }
  }
  return false;
}

void game_debugState() {
  switch (currentState) {
  case init_st:
    printf(INIT_ST_MSG);
    break;
  case welcome_st:
    printf(WELCOME_ST_MSG);
    break;
  case welcome_adc_st:
    printf(WELCOME_ADC_ST_MSG);
    break;
  case play_st:
    printf(PLAY_ST_MSG);
    break;
  case next_level_st:
    printf(NEXT_LEVEL_ST_MSG);
    break;
  case death_st:
    printf(DEATH_ST_MSG);
    break;
  case game_over_st:
    printf(GAME_OVER_ST_MSG);
    break;
  case play_again_st:
    printf(PLAY_AGAIN_ST_MSG);
    break;
  case play_again_adc_st:
    printf(PLAY_AGAIN_ADC_ST_MSG);
    break;
  default:
    printf(ERROR_ST_MSG);
    break;
  }
}

// Standard tick function.
void game_tick() {
  if (enabled) {
    game_debugState();
  }
  switch (currentState) {
  case init_st:
    if (enabled) {
      game_drawWelcome(true);
      nextState = welcome_st;
    } else {
      nextState = init_st;
    }
    break;
  case welcome_st:
    if (!enabled) {
      game_drawWelcome(false);
      nextState = init_st;
    } else if (display_isTouched()) {
      nextState = welcome_adc_st;
    } else {
      nextState = welcome_st;
    }
    break;
  case welcome_adc_st:
    if (!enabled) {
      game_drawWelcome(false);
      nextState = init_st;
    } else if (adcCounter >= ADC_COUNTER_MAX && display_isTouched()) {
      adcCounter = 0;
      game_drawWelcome(false);
      asteroid_enable();
      laser_enable();
      spaceship_enable();
      game_drawScore(true);
      game_drawLives(true);
      asteroid_generateAsteroids(level);
      nextState = play_st;
    } else if (adcCounter >= ADC_COUNTER_MAX && !display_isTouched()) {
      adcCounter = 0;
      nextState = welcome_st;
    } else {
      nextState = welcome_adc_st;
    }
    break;
  case play_st:
    if (!enabled) {
      asteroid_disable();
      laser_disable();
      spaceship_disable();
      game_drawScore(false);
      game_drawLives(false);
      nextState = init_st;
    } else if (asteroid_getCount() == 0 &&
               nextLevelCounter >= NEXT_LEVEL_COUNTER_MAX && lives != 0) {
      asteroid_disable();
      laser_disable();
      nextLevelCounter = 0;
      level++;
      nextState = next_level_st;
    } else {
      if (refreshCounter >= REFRESH_COUNTER_MAX) {
        game_drawScore(true);
        game_drawLives(true);
        refreshCounter = 0;
      }
      game_shipControl();
      game_checkLaserCollision();
      bool shipCollide = game_checkShipCollision();
      if (shipCollide) {
        spaceship_disable();
        nextState = death_st;
      } else {
        nextState = play_st;
      }
      // check ship for collision, if collision remove life and go to death
      // state if no lives, go to game over state
      // check each asteroid for collision, if collision add to score
      // move ship
      // move each asteroid
      // if no asteroids on screen, disable asteroids and go to next level state
    }
    break;
  case next_level_st:
    if (!enabled) {
      asteroid_disable();
      laser_disable();
      spaceship_disable();
      game_drawScore(false);
      game_drawLives(false);
      nextState = init_st;
    } else if (nextLevelCounter >= NEXT_LEVEL_COUNTER_MAX) {
      nextLevelCounter = 0;
      asteroid_enable();
      laser_enable();
      asteroid_generateAsteroids(level);
      nextState = play_st;
    } else {
      nextState = next_level_st;
    }
    break;
  case death_st:
    if (!enabled) {
      asteroid_disable();
      laser_disable();
      spaceship_disable();
      game_drawScore(false);
      game_drawLives(false);
      nextState = init_st;
    } else if (lives == 0 && deathCounter >= DEATH_COUNTER_MAX) {
      asteroid_disable();
      laser_disable();
      spaceship_disable();
      game_drawScore(false);
      game_drawLives(false);
      game_drawGameOver(true);
      deathCounter = 0;
      nextState = game_over_st;
    } else if (deathCounter >= DEATH_COUNTER_MAX) {
      spaceship_enable();
      deathCounter = 0;
      nextState = play_st;
    } else {
      // flash ship, invulnerability?
      nextState = death_st;
    }
    break;
  case game_over_st:
    if (!enabled) {
      game_drawGameOver(false);
      nextState = init_st;
    } else if (gameOverCounter >= GAME_OVER_COUNTER_MAX) {
      asteroid_disable();
      laser_disable();
      spaceship_disable();
      gameOverCounter = 0;
      game_drawPlayAgain(true);
      level = 1;
      nextState = play_again_st;
    } else {
      nextState = game_over_st;
    }
    break;
  case play_again_st:
    if (!enabled) {
      game_drawGameOver(false);
      game_drawPlayAgain(false);
      nextState = init_st;
    } else if (playAgainCounter >= PLAY_AGAIN_COUNTER_MAX) {
      playAgainCounter = 0;
      game_drawGameOver(false);
      game_drawPlayAgain(false);
      game_drawWelcome(true);
      nextState = welcome_st;
    } else if (display_isTouched()) {
      playAgainCounter = 0;
      nextState = play_again_adc_st;
    } else {
      nextState = play_again_st;
    }
    break;
  case play_again_adc_st:
    if (!enabled) {
      game_drawGameOver(false);
      game_drawPlayAgain(false);
      nextState = init_st;
    } else if (adcCounter == ADC_COUNTER_MAX && display_isTouched()) {
      adcCounter = 0;
      game_drawGameOver(false);
      game_drawPlayAgain(false);
      score = 0;
      lives = START_LIVES;
      game_drawLives(true);
      game_drawScore(true);
      asteroid_enable();
      laser_enable();
      spaceship_enable();
      nextState = play_st;
    } else if (adcCounter == ADC_COUNTER_MAX && !display_isTouched()) {
      adcCounter = 0;
      score = 0;
      game_drawGameOver(false);
      game_drawPlayAgain(false);
      game_drawWelcome(true);
      nextState = init_st;
    } else {
      nextState = play_again_adc_st;
    }
    break;
  default:
    break;
  }
  currentState = nextState;
  switch (currentState) {
  case init_st:
    level = 1;
    refreshCounter = 0;
    deathCounter = 0;
    nextLevelCounter = 0;
    adcCounter = 0;
    score = 0;
    playAgainCounter = 0;
    break;
  case welcome_st:
    break;
  case welcome_adc_st:
    adcCounter++;
    break;
  case play_st:
    refreshCounter++;
    nextLevelCounter++;
    break;
  case next_level_st:
    nextLevelCounter++;
    break;
  case death_st:
    deathCounter++;
    break;
  case game_over_st:
    gameOverCounter++;
    break;
  case play_again_st:
    playAgainCounter++;
    break;
  case play_again_adc_st:
    adcCounter++;
    break;
  default:
    break;
  }
}

// Enable the state machine (interlock).
void game_enable() { enabled = true; }

// Disable the state machine (interlock).
void game_disable() { enabled = false; }

// Use this predicate to see if the game is finished.
bool game_isGameOver() { return lives == 0; }