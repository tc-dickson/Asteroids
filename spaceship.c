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
  5.0 // Value in degrees. This value will be used in the rotation matrix to
      // rotate the verticies of the spaceship by this amount each tick.

#define ROTATION_ANGLE_CHANGE_RAD                                              \
  ((ROTATION_ANGLE_CHANGE_DEG * PI) /                                          \
   180.0) // Conversion from degrees to radians. The c standard library
          // functions
          // of sine and cosine take arguments that are in units of radians.

#define SCALING_FACTOR                                                         \
  10000 // Use 10^4 as the scaling factor for fixed point formatting.

// Definitions for the positioning of the verticies of the spaceship.
#define NUM_VERTICIES 5

// Starting values for the vertecies of the spaceship represented as x, y pairs
// (vector).
#define VERTEX_1_X 0.0
#define VERTEX_1_Y -10.0

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

// Create a struct to hold the information of the spaceship.
typedef struct {
  vector2D_t centerPoint;
  uint8_t numVerticies; // Length of vertexArr.
  vector2D_t vertexArr[NUM_VERTICIES];
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
  spaceship.centerPoint.x = CENTER_X;
  spaceship.centerPoint.y = CENTER_Y;

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

  printf("This is the value of rotationCCW matrix [ %d, %d; %d, %d]\n",
         rotationCCW.vect1.x, rotationCCW.vect2.x, rotationCCW.vect1.y,
         rotationCCW.vect2.y);
  printf("This is the value of rotationCW matrix [ %d, %d; %d, %d]\n",
         rotationCW.vect1.x, rotationCW.vect2.x, rotationCW.vect1.y,
         rotationCW.vect2.y);
#if 1
  // Repeatedly perform the rotation to test its functionality.
  while (true) {
    // Print the values in the vertexArr within the spaceship struct.
    for (uint8_t i = 0; i < spaceship.numVerticies; i++) {
      printf("This is the value of vertex%d: (%d, %d)\n", i + 1,
             spaceship.vertexArr[i].x, spaceship.vertexArr[i].y);
    }
    printf("\n"); // Print a newline character.

    // Draw the spaceship
    spaceship_drawShip(DRAW_VALUE);

    // Wait a prescribed amount of time.
    utils_msDelay(DELAY_TIME_MS);

    // Erase the previously drawn spaceship.
    spaceship_drawShip(ERASE_VALUE);

    // Rotate the spaceship CCW.
    spaceship_rotateShip(rotateCCW);
  }
#endif
}

// Draw the spaceship if the parameter draw is true. Otherwise erase the ship.
void spaceship_drawShip(bool draw) {
  // Loop through the spaceship.vertexArr to get the points for the lines that
  // make up the body of the ship. This will result in a closed path.
  for (uint8_t i = 0; i < spaceship.numVerticies; i++) {
    // Use the vertex at index i and the the one at vertex i + 1 to draw the
    // lines as long as i + 1 is within the size of vertexArr.
    if (i < spaceship.numVerticies - 1) {
      printf(
          "line (x1, y1), (x2, y2): (%d, %d), (%d, %d)\n",
          spaceship.centerPoint.x +
              (spaceship.vertexArr[i].x / spaceship.vertexArr[i].scalingFactor),
          spaceship.centerPoint.y +
              (spaceship.vertexArr[i].y / spaceship.vertexArr[i].scalingFactor),
          spaceship.centerPoint.x + (spaceship.vertexArr[i + 1].x /
                                     spaceship.vertexArr[i + 1].scalingFactor),
          spaceship.centerPoint.y + (spaceship.vertexArr[i + 1].y /
                                     spaceship.vertexArr[i + 1].scalingFactor));

      display_drawLine(
          spaceship.centerPoint.x +
              (spaceship.vertexArr[i].x / spaceship.vertexArr[i].scalingFactor),
          spaceship.centerPoint.y +
              (spaceship.vertexArr[i].y / spaceship.vertexArr[i].scalingFactor),
          spaceship.centerPoint.x + (spaceship.vertexArr[i + 1].x /
                                     spaceship.vertexArr[i + 1].scalingFactor),
          spaceship.centerPoint.y + (spaceship.vertexArr[i + 1].y /
                                     spaceship.vertexArr[i + 1].scalingFactor),
          draw ? SPACESHIP_COLOR : ERASE_COLOR);
    } else { // Use the 1st index as the second x, y pair for the line if i + 1
             // is equal to spaceship.numVerticies.
      display_drawLine(
          spaceship.centerPoint.x +
              (spaceship.vertexArr[i].x / spaceship.vertexArr[i].scalingFactor),
          spaceship.centerPoint.y +
              (spaceship.vertexArr[i].y / spaceship.vertexArr[i].scalingFactor),
          spaceship.centerPoint.x +
              (spaceship.vertexArr[0].x / spaceship.vertexArr[0].scalingFactor),
          spaceship.centerPoint.y +
              (spaceship.vertexArr[0].y / spaceship.vertexArr[0].scalingFactor),
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
