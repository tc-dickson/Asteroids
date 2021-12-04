#ifndef LINEARALG_H_
#define LINEARALG_H_

#include <stdint.h>

// Declaration of a struct to hold a 2D vector.
typedef struct {
  uint16_t x; // Use a 16 bit number to hold all the possible values for the x-y
              // coordinates of the LCD screen.
  uint16_t y;
} vector2D;

// Declaration of a struct to hold a 2x2 matrix.
typedef struct {
  vector2D vect1;
  vector2D vect2;
} matrix2x2;

// A function that calculates the given vector left multiplied by the given
// matrix. This will be used to calculate rotations.
vector2D matVectMultAx2D(matrix2x2, vector2D);

#endif // LINEARALG_H_