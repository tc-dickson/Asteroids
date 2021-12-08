#include "linearAlg.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#define SQUARE_EXP 2 // The value of the exponent to square a number.

// Function Declarations.
elementSize_t dotProductFixedPoint(vector2D_t vect1, vector2D_t vect2);

// A function that calculates the given vector left multiplied by the given
// matrix. This will be used to calculate rotations.
void linearAlg_matVectMultAx2DFixedPoint(matrix2x2_t matrix,
                                         vector2D_t *vector) {
  // Calculate the vector left multiplied by the matrix.
  vector2D_t tempVector;

  tempVector.x = dotProductFixedPoint(
      (vector2D_t){.x = matrix.vect1.x, .y = matrix.vect2.x}, *vector);
  tempVector.y = dotProductFixedPoint(
      (vector2D_t){.x = matrix.vect1.y, .y = matrix.vect2.y}, *vector);
  *vector = tempVector;
}

// Calculate the dot product of two vectors vect1 and vect2.
elementSize_t dotProductFixedPoint(vector2D_t vect1, vector2D_t vect2) {
  return ((vect1.x * vect2.x) + (vect1.y * vect2.y));
}

// Return the magnitude of a vector of type vector2D_t.
elementSize_t linearAlg_calcMag(vector2D_t vector) {
  return sqrt((vector.x) * (vector.x)) + ((vector.y) * (vector.y));
}

// Return a normalized vector based upon the direction and magnitude.
vector2D_t linearAlg_normVect(vector2D_t vector, elementSize_t magnitude) {
  // Check if the magnitude is greater than to zero. If so then return the
  // normal. Else return a zero vector.
  if (magnitude > 0) {
    return (vector2D_t){.x = (vector.x / magnitude),
                        .y = (vector.y / magnitude)};
  } else {
    return (vector2D_t){.x = 0, .y = 0};
  }
}