#include "linearAlg.h"
#include <stdint.h>

// Function Declarations.
uint16_t dotProduct(vector2D_t vect1, vector2D_t vect2);

// A function that calculates the given vector left multiplied by the given
// matrix. This will be used to calculate rotations.
void linearAlg_matVectMultAx2D(matrix2x2_t matrix, vector2D_t *vector) {
  // Calculate the vector left multiplied by the matrix.
  vector2D_t returnVect;
  returnVect.x = dotProduct(
      (vector2D_t){.x = matrix.vect1.x, .y = matrix.vect2.x}, *vector);
  returnVect.y = dotProduct(
      (vector2D_t){.x = matrix.vect1.y, .y = matrix.vect2.y}, *vector);
}

// Calculate the dot product of two vectors vect1 and vect2.
uint16_t dotProduct(vector2D_t vect1, vector2D_t vect2) {
  return (vect1.x * vect2.x) + (vect1.y * vect2.y);
}