#ifndef LINEARALG_H_
#define LINEARALG_H_

#include <stdint.h>

typedef int64_t elementSize_t;

// Declaration of a struct to hold a 2D vector.
typedef struct {
  elementSize_t x;
  elementSize_t y;
  elementSize_t scalingFactor; // Use this number to facilitate fixed point
                               // representations of floating point numbers.
                               // Assume that all numbers in this struct have
                               // been multiplied by this scaling factor.
} vector2D_t;

// Declaration of a struct to hold a 2x2 matrix.
typedef struct {
  vector2D_t vect1;
  vector2D_t vect2;
  elementSize_t scalingFactor; // Use this number to facilitate fixed point
                               // representations of floating point numbers.
                               // Assume that all numbers in this struct have
                               // been multiplied by this scaling factor.
} matrix2x2_t;

// A function that calculates the given vector left multiplied by the given
// matrix. This will be used to calculate rotations.
void linearAlg_matVectMultAx2DFixedPoint(matrix2x2_t, vector2D_t *);

// Return the magnitude of a vector of type vector2D_t.
elementSize_t linearAlg_calcMag(vector2D_t vector);

// Return a normalized vector based upon the direction and magnitude.
vector2D_t linearAlg_normVect(vector2D_t vector, elementSize_t magnitude);

#endif // LINEARALG_H_