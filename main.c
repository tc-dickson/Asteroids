/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#include <stdbool.h>
#include <stdio.h>

#include "asteroid.h"
#include "buttons.h"
#include "config.h"
#include "display.h"
#include "game.h"
#include "interrupts.h"
#include "intervalTimer.h"
#include "laser.h"
#include "leds.h"
#include "spaceship.h"
#include "utils.h"
#include "xparameters.h"

// Compute the timer clock freq.
#define TIMER_CLOCK_FREQUENCY (XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ / 2)
// Compute timer load value.
#define TIMER_LOAD_VALUE ((CONFIG_TIMER_PERIOD * TIMER_CLOCK_FREQUENCY) - 1.0)

#define INTERRUPTS_PER_SECOND (1.0 / CONFIG_TIMER_PERIOD)
#define TOTAL_SECONDS 45
#define MAX_INTERRUPT_COUNT (INTERRUPTS_PER_SECOND * TOTAL_SECONDS)

#define DISPLAY_MID_X DISPLAY_WIDTH / 2
#define DISPLAY_MID_Y DISPLAY_HEIGHT / 2

// The formula for computing the load value is based upon the formula from 4.1.1
// (calculating timer intervals) in the Cortex-A9 MPCore Technical Reference
// Manual 4-2. Assuming that the prescaler = 0, the formula for computing the
// load value based upon the desired period is: load-value = (period *
// timer-clock) - 1
#define TIMER_CLOCK_FREQUENCY (XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ / 2)
#define TIMER_LOAD_VALUE ((CONFIG_TIMER_PERIOD * TIMER_CLOCK_FREQUENCY) - 1.0)

#define MAX_ACTIVE_MOLES 1 // Start out with this many moles.
#define MAX_MISSES 50      // Game is over when there are this many misses.
#define FOREVER 1          // Syntactic sugar for while (1) statements.
#define MS_PER_TICK                                                            \
  (CONFIG_TIMER_PERIOD *                                                       \
   1000) // Just multiply the timer period by 1000 to get ms.

#define SWITCH_VALUE_9 9 // Binary 9 on the switches indicates 9 moles.
#define SWITCH_VALUE_6 6 // Binary 6 on the switches indicates 6 moles.
#define SWITCH_VALUE_4 4 // Binary 9 on the switches indicates 4 moles.
#define SWITCH_MASK 0xf  // Ignore potentially extraneous bits.

static uint32_t randomSeed; // Used to make the game seem more random.

static void test_init() {
  asteroid_init();
  laser_init();
  spaceship_init();
  game_init();
}

void tickAll() {
  asteroid_tick();
  laser_tick();
  spaceship_tick();
  game_tick();
}

int main() {
  test_init();
  game_enable();
  interrupts_initAll(true);
  interrupts_setPrivateTimerLoadValue(TIMER_LOAD_VALUE);
  interrupts_enableTimerGlobalInts();
  // Keep track of your personal interrupt count. Want to make sure that you
  // don't miss any interrupts.
  int32_t personalInterruptCount = 0;
  // Start the private ARM timer running.
  interrupts_startArmPrivateTimer();
  // Enable interrupts at the ARM.
  interrupts_enableArmInts();
  uint8_t counter = 0;
  while (1) {
    counter++;
    if (interrupts_isrFlagGlobal) {
      // Count ticks.
      // personalInterruptCount++;
      tickAll();
      interrupts_isrFlagGlobal = 0;
      if (personalInterruptCount >= MAX_INTERRUPT_COUNT)
        break;
      utils_sleep();
    }
  }
  interrupts_disableArmInts();
  printf("isr invocation count: %d\n", interrupts_isrInvocationCount());
  printf("internal interrupt count: %d\n", personalInterruptCount);
  return 0;
}

void isr_function() {}
