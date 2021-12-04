#ifndef LINEARALG_H_
#define LINEARALG_H_

#include <stdint.h>

// Declaration of a struct to hold a 2D vector.
typedef struct {
  int16_t x; // Use a 16 bit number to hold all the possible values for the x-y
             // coordinates of the LCD screen.
  int16_t y;
} vector2D_t;

// Declaration of a struct to hold a 2x2 matrix.
typedef struct {
  vector2D_t vect1;
  vector2D_t vect2;
} matrix2x2_t;

// A function that calculates the given vector left multiplied by the given
// matrix. This will be used to calculate rotations.
vector2D_t matVectMultAx2D(matrix2x2_t, vector2D_t);

#endif // LINEARALG_H_