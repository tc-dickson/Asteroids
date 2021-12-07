#include "spaceship.h"
#include "display.h"
#include "linearAlg.h"
#include "utils.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define SPACESHIP_COLOR DISPLAY_WHITE
#define ERASE_COLOR DISPLAY_BLACK
#define DRAW_VALUE true
#define ERASE_VALUE false

#define DELAY_TIME_MS 50 // Wait 50ms.

// Definitions for rotation.
#define PI 3.14159265358979323846

#define ROTATION_ANGLE_CHANGE_DEG                                              \
  15.0 // Value in degrees. This value will be used in the rotation matrix to
       // rotate the verticies of the spaceship by this amount each tick.

#define ROTATION_ANGLE_CHANGE_RAD                                              \
  ((ROTATION_ANGLE_CHANGE_DEG * PI) /                                          \
   180.0) // Conversion from degrees to radians. The c standard library
          // functions
          // of sine and cosine take arguments that are in units of radians.

#define SCALING_FACTOR                                                         \
  1e8 // Use 1e8 as the scaling factor for fixed point formatting. Use such a
      // large number to prevent rounding errors that result in strange behavior
      // such as shrinking the size of the vectors after repeated operations.

// Definitions for translational movement.
#define ACCELERATION                                                           \
  2 // Number of units to add to the velocity vector each tick.
#define MAX_VELOCITY                                                           \
  15 // This is the maximum amplitude of the velocity vector. This is also the
     // maximum number of units the spaceship will travel each tick. This number
     // acts as a drag coefficient of sorts where the drag equation is Cd *
     // (V^2) / 2. The MAX_VELOCITY definition combines the values of 1 / 2 and
     // Cd into a single value.

// Find the magnitude of the drag by finding the velocity (which can vary)
// squared divided by the maximum velocity.
#define DRAG_MAGNITUDE(A) ((A * A) / MAX_VELOCITY)

// Definitions for the positioning of the verticies of the spaceship.
#define NUM_VERTICIES 5

// Starting values for the vertecies of the spaceship represented as x, y pairs
// (vector).
#define VERTEX_1_X 0.0
#define VERTEX_1_Y -10.0

// Vertex 1 is also the starting direction vector.
#define DIRECT_VECT_X VERTEX_1_X
#define DIRECT_VECT_Y VERTEX_1_Y

#define VERTEX_2_X 7.0
#define VERTEX_2_Y 10.0

#define VERTEX_3_X 3.0
#define VERTEX_3_Y 5.0

#define VERTEX_4_X -3.0
#define VERTEX_4_Y 5.0

#define VERTEX_5_X -7.0
#define VERTEX_5_Y 10.0

#define CENTER_X (display_width() / 2)
#define CENTER_Y (display_height() / 2)

// Function Declarations.
void updateVectors(vector2D_t *centerPoint, vector2D_t *velocityVect,
                   vector2D_t thrustVect, bool applyThrustVector);

// Create a struct to hold the information of the spaceship.
typedef struct {
  vector2D_t centerPoint; // Coordinates of the center point of the spaceship.
                          // This is used to base the position of the vectors
                          // that compose the lines of the spaceship. This is
                          // also used to calculate the movement of the ship.
  uint8_t numVerticies;   // Length of vertexArr.
  vector2D_t vertexArr[NUM_VERTICIES]; // Hold the positions of the spaceship's
                                       // vectors relative to the center point.
  vector2D_t thrustVect;   // Holds the x, y, coordinates of the to be added to
                           // the velocity vector when the spaceship has its
                           // "rockets" on.
  elementSize_t thrustMag; // This value holds the magnitude of the thrustVect.
  vector2D_t velocityVect;
  elementSize_t
      velocityMag; // This value holds the magnitude of the velocityVect.
} spaceship_t;

// Declare a spaceship_t variable.
spaceship_t spaceship;

// Declare matrix variables to hold the values of the rotation matricies in the
// CCW and CW directions.
matrix2x2_t rotationCCW;
matrix2x2_t rotationCW;

// Initialize the spaceship with starting values. Create the rotation matricies
// for CCW and CW rotation.
void spaceship_init() {
  // Initialize the center point vector.
  spaceship.centerPoint.x = CENTER_X * SCALING_FACTOR;
  spaceship.centerPoint.y = CENTER_Y * SCALING_FACTOR;
  spaceship.centerPoint.scalingFactor = SCALING_FACTOR;

  spaceship.numVerticies = NUM_VERTICIES;

  // Assign all of vertexArr to constant pre-defined values. Store the values in
  // a fixed point format (i.e. scale them all by SCALING_FACTOR).
  memcpy(spaceship.vertexArr,
         (const vector2D_t[]){(vector2D_t){.x = VERTEX_1_X * SCALING_FACTOR,
                                           .y = VERTEX_1_Y * SCALING_FACTOR,
                                           .scalingFactor = SCALING_FACTOR},
                              (vector2D_t){.x = VERTEX_2_X * SCALING_FACTOR,
                                           .y = VERTEX_2_Y * SCALING_FACTOR,
                                           .scalingFactor = SCALING_FACTOR},
                              (vector2D_t){.x = VERTEX_3_X * SCALING_FACTOR,
                                           .y = VERTEX_3_Y * SCALING_FACTOR,
                                           .scalingFactor = SCALING_FACTOR},
                              (vector2D_t){.x = VERTEX_4_X * SCALING_FACTOR,
                                           .y = VERTEX_4_Y * SCALING_FACTOR,
                                           .scalingFactor = SCALING_FACTOR},
                              (vector2D_t){.x = VERTEX_5_X * SCALING_FACTOR,
                                           .y = VERTEX_5_Y * SCALING_FACTOR,
                                           .scalingFactor = SCALING_FACTOR}},
         sizeof spaceship.vertexArr);

  // Assign the rotation matricies with appropriate values. The matrix that
  // rotates vectors when multiplied is of the form:
  // [cos(theta), -sin(theta)
  //  sin(theta),  cos(theta)]
  // This matrix can be thought of an array of 2 vectors with the first having
  // elements of the first column of the matrix and the second vector having
  // elements of the second column.
  // Because the coordinate system of the display
  // is a left-handed coordinate system a rotation of a positive angle will be
  // in the CW direction.
  // The return type of the trig functions is double, but we will use a fixed
  // point conversion using the scaling factor built into the matricies.
  rotationCCW.scalingFactor = SCALING_FACTOR;
  rotationCCW.vect1.x = (elementSize_t)(cos(-ROTATION_ANGLE_CHANGE_RAD) *
                                        rotationCCW.scalingFactor);
  rotationCCW.vect1.y = (elementSize_t)(sin(-ROTATION_ANGLE_CHANGE_RAD) *
                                        rotationCCW.scalingFactor);
  rotationCCW.vect2.x = (elementSize_t)(-sin(-ROTATION_ANGLE_CHANGE_RAD) *
                                        rotationCCW.scalingFactor);
  rotationCCW.vect2.y = (elementSize_t)(cos(-ROTATION_ANGLE_CHANGE_RAD) *
                                        rotationCCW.scalingFactor);

  rotationCW.scalingFactor = SCALING_FACTOR;
  rotationCW.vect1.x = (elementSize_t)(cos(ROTATION_ANGLE_CHANGE_RAD) *
                                       rotationCW.scalingFactor);
  rotationCW.vect1.y = (elementSize_t)(sin(ROTATION_ANGLE_CHANGE_RAD) *
                                       rotationCW.scalingFactor);
  rotationCW.vect2.x = (elementSize_t)(-sin(ROTATION_ANGLE_CHANGE_RAD) *
                                       rotationCW.scalingFactor);
  rotationCW.vect2.y = (elementSize_t)(cos(ROTATION_ANGLE_CHANGE_RAD) *
                                       rotationCW.scalingFactor);
}

// Use this function to test various parts of the spaceship code.
void spaceship_runTest(bool rotateCCW) {
  // Perform initialization.
  spaceship_init();

  // Repeatedly perform the rotation to test its functionality.
  while (true) {
    // Draw the spaceship
    spaceship_drawShip(DRAW_VALUE);

    // Wait a prescribed amount of time.
    utils_msDelay(DELAY_TIME_MS);

    // Erase the previously drawn spaceship.
    spaceship_drawShip(ERASE_VALUE);

    // Rotate the spaceship CCW.
    spaceship_rotateShip(rotateCCW);
  }
}

// Draw the spaceship if the parameter draw is true. Otherwise erase the ship.
void spaceship_drawShip(bool draw) {
  // Loop through the spaceship.vertexArr to get the points for the lines that
  // make up the body of the ship. This will result in a closed path.
  for (uint8_t i = 0; i < spaceship.numVerticies; i++) {
    // Use the vertex at index i and the the one at vertex i + 1 to draw the
    // lines as long as i + 1 is within the size of vertexArr.
    if (i < spaceship.numVerticies - 1) {
      display_drawLine(
          (spaceship.centerPoint.x + spaceship.vertexArr[i].x) /
              spaceship.vertexArr[i].scalingFactor,
          (spaceship.centerPoint.y + spaceship.vertexArr[i].y) /
              spaceship.vertexArr[i].scalingFactor,
          (spaceship.centerPoint.x + spaceship.vertexArr[i + 1].x) /
              spaceship.vertexArr[i + 1].scalingFactor,
          (spaceship.centerPoint.y + spaceship.vertexArr[i + 1].y) /
              spaceship.vertexArr[i + 1].scalingFactor,
          draw ? SPACESHIP_COLOR : ERASE_COLOR);
    } else { // Use the 1st index as the second x, y pair for the line if i + 1
             // is equal to spaceship.numVerticies.
      display_drawLine((spaceship.centerPoint.x + spaceship.vertexArr[i].x) /
                           spaceship.vertexArr[i].scalingFactor,
                       (spaceship.centerPoint.y + spaceship.vertexArr[i].y) /
                           spaceship.vertexArr[i].scalingFactor,
                       (spaceship.centerPoint.x + spaceship.vertexArr[0].x) /
                           spaceship.vertexArr[0].scalingFactor,
                       (spaceship.centerPoint.y + spaceship.vertexArr[0].y) /
                           spaceship.vertexArr[0].scalingFactor,
                       draw ? SPACESHIP_COLOR : ERASE_COLOR);
    }
  }
}

// Function to rotate the spaceship. If rotateCC is true then the spaceship will
// rotate counter-clockwise. Otherwise it will rotate clockwise.
void spaceship_rotateShip(bool rotateCCW) {
  // If rotateCCW is true, left multiply the vectors in the vectorArr of
  // spaceship by the rotationCCW matrix.
  if (rotateCCW) {
    for (uint8_t i = 0; i < spaceship.numVerticies; i++) {
      linearAlg_matVectMultAx2DFixedPoint(rotationCCW,
                                          &(spaceship.vertexArr[i]));
    }
  } else { // Otherwise multiply by the rotationCW matrix.
    for (uint8_t i = 0; i < spaceship.numVerticies; i++) {
      linearAlg_matVectMultAx2DFixedPoint(rotationCW,
                                          &(spaceship.vertexArr[i]));
    }
  }
}

// Function to move the spaceship forward in the direction it is facing if the
// rocketsAreFiring parameter is true. The spaceship will continue to coast for
// a time after the rockets are turned off (with rocketsAreFiring parameter
// being false).
void spaceship_rockets(bool rocketsAreFiring) {}

// Calculates the next position for the rocket based upon the calculations of
// thrust vector (if applyThrustVector is true), velocity vector, and drag
// vector. The drag vector is dependent upon the velocity vector so it is not a
// parameter to the argument. Use pointers to update the centerPoint vector and
// the velocity vector of the spaceship directly.
void updateVectors(vector2D_t *centerPoint, vector2D_t *velocityVect,
                   vector2D_t thrustVect, bool applyThrustVector) {
  // Find the magnitude of the velocityVect.
  elementSize_t velocityVectMag = linearAlg_calcMag(*velocityVect);

  // Generate the drag vector by first normalizing the velocity vector.
  vector2D_t dragVect = linearAlg_normVect(*velocityVect, velocityVectMag);
  // Next, invert the sign of both the x and y components of the vectors and
  // multiply them by the magnitude of the drag.
  dragVect.x = -dragVect.x * DRAG_MAGNITUDE(velocityVectMag);
  dragVect.y = -dragVect.y * DRAG_MAGNITUDE(velocityVectMag);

  // Add the vectors together to calculate the new velocity vector. Only add the
  // thrust vector components if the bool applyThrustVector is true.
  velocityVect->x += (applyThrustVector ? dragVect.x : 0) + thrustVect.x;
  velocityVect->y += (applyThrustVector ? dragVect.y : 0) + thrustVect.y;

  // Calculate the updated center points based upon the new velocityVect.
  centerPoint->x += velocityVect->x;
  centerPoint->y += velocityVect->y;
}