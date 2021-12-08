#include "spaceship.h"
#include <stdbool.h>

int main() {
  // Test the spaceship code.
  spaceship_runTest(false, true);
}

// Interrupt routine
void isr_function() {
  // Empty for flag method (flag set elsewhere)
}