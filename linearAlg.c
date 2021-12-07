#include "linearAlg.h"
#include <stdint.h>
#include <stdio.h>

// Function Declarations.
elementSize_t dotProductFixedPoint(vector2D_t vect1, vector2D_t vect2,
                                   elementSize_t scalingFactor);

// A function that calculates the given vector left multiplied by the given
// matrix. This will be used to calculate rotations.
void linearAlg_matVectMultAx2DFixedPoint(matrix2x2_t matrix,
                                         vector2D_t *vector) {
  // Calculate the vector left multiplied by the matrix.
  vector2D_t tempVector;

  // Be certain to set tempVector's scalingFactor
  tempVector.scalingFactor = vector->scalingFactor;

  tempVector.x = dotProductFixedPoint(
      (vector2D_t){.x = matrix.vect1.x, .y = matrix.vect2.x}, *vector,
      matrix.scalingFactor);
  tempVector.y = dotProductFixedPoint(
      (vector2D_t){.x = matrix.vect1.y, .y = matrix.vect2.y}, *vector,
      matrix.scalingFactor);
  *vector = tempVector;
}

// Calculate the dot product of two vectors vect1 and vect2.
elementSize_t dotProductFixedPoint(vector2D_t vect1, vector2D_t vect2,
                                   elementSize_t scalingFactor) {
  return ((vect1.x * vect2.x) + (vect1.y * vect2.y)) / scalingFactor;
}