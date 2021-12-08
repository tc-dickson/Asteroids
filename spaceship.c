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

// Definitions for translational movement.
#define ACCELERATION                                                           \
  200 // Number of units to add to the velocity vector each
      // tick.

#define MAX_VELOCITY                                                           \
  15 // This is the maximum amplitude of the velocity vector.
     // This is also the maximum number of units the spaceship
     // will travel each tick. This number acts as a drag
     // coefficient of sorts where the drag equation is Cd *
     // (V^2) / 2. The MAX_VELOCITY definition combines the
     // values of 1 / 2 and Cd into a single value.

#define DRAG_MAGNITUDE(velocity)                                               \
  ((velocity * velocity) /                                                     \
   MAX_VELOCITY) // Find the magnitude of the drag by finding the
                 // velocity (which can vary) squared divided by the
                 // maximum velocity.

#define FIRST_INDEX                                                            \
  0 // Use this to specifically say that the direction vector is contained in
    // the first index of a larger array.

// Starting values for the vertecies of the spaceship represented as x, y pairs
// (vector).
#define VERTEX_1_X 0.0
#define VERTEX_1_Y -10.0

// Vertex 1 is also the starting direction vector.
// #define DIRECT_VECT_X VERTEX_1_X
// #define DIRECT_VECT_Y VERTEX_1_Y

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
  vector2D_t centerPoint; // Coordinates of the center point of the spaceship.
                          // This is used to base the position of the vectors
                          // that compose the lines of the spaceship. This is
                          // also used to calculate the movement of the ship.
  uint8_t numVerticies;   // Length of vectorArr.
  vector2D_t vectorArr[NUM_VERTICIES]; // Hold the positions of the spaceship's
                                       // vectors relative to the center point.
  elementSize_t directVectMag;         // Holds the magnitude of the direction
                                       // vector which will not change.
  vector2D_t thrustVect; // Holds the x, y, coordinates of the to be added to
                         // the velocity vector when the spaceship has its
                         // "rockets" on.
  elementSize_t thrustVectMag; // This value holds the magnitude of the
                               // thrustVect which will be a predetermined
                               // constant value that will be initialized.
  vector2D_t
      velocityVect; // This holds the information for the velocity vector.
} spaceship_t;

// Function Declarations.
void updateVectors(spaceship_t *spaceship, bool applyThrustVector);

// Declare a spaceship_t variable.
spaceship_t spaceship;

// Declare matrix variables to hold the values of the rotation matricies in the
// counter clock-wise (CCW) and clock-wise (CW) directions.
matrix2x2_t rotationCCW;
matrix2x2_t rotationCW;

// Initialize the spaceship with starting values. Create the rotation matricies
// for CCW and CW rotation.
void spaceship_init() {
  // Initialize the center point vector.
  spaceship.centerPoint.x = CENTER_X;
  spaceship.centerPoint.y = CENTER_Y;

  // The number of points comprising the spaceship.
  spaceship.numVerticies = NUM_VERTICIES;

  // Assign all of vectorArr to constant pre-defined values. Store the values in
  // a fixed point format (i.e. scale them all by SCALING_FACTOR).
  memcpy(spaceship.vectorArr,
         (const vector2D_t[]){(vector2D_t){.x = VERTEX_1_X, .y = VERTEX_1_Y},
                              (vector2D_t){.x = VERTEX_2_X, .y = VERTEX_2_Y},
                              (vector2D_t){.x = VERTEX_3_X, .y = VERTEX_3_Y},
                              (vector2D_t){.x = VERTEX_4_X, .y = VERTEX_4_Y},
                              (vector2D_t){.x = VERTEX_5_X, .y = VERTEX_5_Y}},
         sizeof spaceship.vectorArr);

  // Initialize the thrust vector using the direction of the first vector in
  // spaceship.vectorArr.
  spaceship.directVectMag = linearAlg_calcMag(spaceship.vectorArr[FIRST_INDEX]);
  spaceship.thrustVect = linearAlg_normVect(spaceship.vectorArr[FIRST_INDEX],
                                            spaceship.directVectMag);

  printf("This is the value of thrustVect x, y at initialization: %lld, %lld\n",
         spaceship.thrustVect.x, spaceship.thrustVect.y);

  spaceship.thrustVectMag = ACCELERATION;

  // Initialize the velocity vector. Starting x, y values should be 0.
  spaceship.velocityVect = (vector2D_t){.x = 0, .y = 0};

  // Assign the rotation matricies with appropriate
  // values. The matrix that rotates vectors when
  // multiplied is of the form:
  // [cos(theta), -sin(theta)
  //  sin(theta),  cos(theta)]
  // This matrix can be thought of an array of 2 vectors
  // with the first having elements of the first column
  // of the matrix and the second vector having elements
  // of the second column. Because the coordinate system
  // of the display is a left-handed coordinate system a
  // rotation of a positive angle will be in the CW
  // direction. The return type of the trig functions is
  // double, but we will use a fixed point conversion
  // using the scaling factor built into the matricies.
  rotationCCW.vect1.x = (elementSize_t)(cos(-ROTATION_ANGLE_CHANGE_RAD));
  rotationCCW.vect1.y = (elementSize_t)(sin(-ROTATION_ANGLE_CHANGE_RAD));
  rotationCCW.vect2.x = (elementSize_t)(-sin(-ROTATION_ANGLE_CHANGE_RAD));
  rotationCCW.vect2.y = (elementSize_t)(cos(-ROTATION_ANGLE_CHANGE_RAD));

  rotationCW.vect1.x = (elementSize_t)(cos(ROTATION_ANGLE_CHANGE_RAD));
  rotationCW.vect1.y = (elementSize_t)(sin(ROTATION_ANGLE_CHANGE_RAD));
  rotationCW.vect2.x = (elementSize_t)(-sin(ROTATION_ANGLE_CHANGE_RAD));
  rotationCW.vect2.y = (elementSize_t)(cos(ROTATION_ANGLE_CHANGE_RAD));
}

// Use this function to test various parts of the spaceship code.
void spaceship_runTest(bool rotateCCW, bool fireRockets) {
  // Perform initialization.
  spaceship_init();

  for (uint8_t i = 0; i < 10; i++) {
    spaceship_rotateShip(rotateCCW);
  }

  // Repeatedly perform the rotation to test its functionality.
  // for (uint8_t i = 0; i < 5; i++) {
  while (true) {
    // Draw the spaceship
    spaceship_drawShip(DRAW_VALUE);

    // Wait a prescribed amount of time.
    utils_msDelay(DELAY_TIME_MS);

    // Erase the previously drawn spaceship.
    spaceship_drawShip(ERASE_VALUE);

    // Rotate the spaceship CCW.
    // spaceship_rotateShip(rotateCCW);

    // Move the spaceship forward.
    spaceship_translateShip(fireRockets);

    printf("Center point x, y: %d, %d\n", spaceship.centerPoint.x,
           spaceship.centerPoint.y);
  }
}

// Draw the spaceship if the parameter draw is true. Otherwise erase the ship.
void spaceship_drawShip(bool draw) {
  // Loop through the spaceship.vectorArr to get the points for the lines that
  // make up the body of the ship. This will result in a closed path.
  for (uint8_t i = 0; i < spaceship.numVerticies; i++) {
    // Use the vertex at index i and the the one at vertex i + 1 to draw the
    // lines as long as i + 1 is within the size of vectorArr.
    if (i < spaceship.numVerticies - 1) {
      display_drawLine((spaceship.centerPoint.x + spaceship.vectorArr[i].x),
                       (spaceship.centerPoint.y + spaceship.vectorArr[i].y),
                       (spaceship.centerPoint.x + spaceship.vectorArr[i + 1].x),
                       (spaceship.centerPoint.y + spaceship.vectorArr[i + 1].y),
                       draw ? SPACESHIP_COLOR : ERASE_COLOR);
    } else { // Use the 1st index as the second x, y pair for the line if i + 1
             // is equal to spaceship.numVerticies.
      display_drawLine((spaceship.centerPoint.x + spaceship.vectorArr[i].x),
                       (spaceship.centerPoint.y + spaceship.vectorArr[i].y),
                       (spaceship.centerPoint.x + spaceship.vectorArr[0].x),
                       (spaceship.centerPoint.y + spaceship.vectorArr[0].y),
                       draw ? SPACESHIP_COLOR : ERASE_COLOR);
    }
  }
}

// Function to rotate the spaceship. If rotateCCW is true then the spaceship
// will rotate counter-clockwise. Otherwise it will rotate clockwise.
void spaceship_rotateShip(bool rotateCCW) {
  // If rotateCCW is true, left multiply the vectors in the vectorArr of
  // spaceship by the rotationCCW matrix.
  if (rotateCCW) {
    for (uint8_t i = 0; i < spaceship.numVerticies; i++) {
      linearAlg_matVectMultAx2DFixedPoint(rotationCCW,
                                          &(spaceship.vectorArr[i]));
    }
  } else { // Otherwise multiply by the rotationCW matrix.
    for (uint8_t i = 0; i < spaceship.numVerticies; i++) {
      linearAlg_matVectMultAx2DFixedPoint(rotationCW,
                                          &(spaceship.vectorArr[i]));
    }
  }
}

// Calculates the next position for the rocket based upon the calculations of
// thrust vector (if applyThrustVector is true), velocity vector, and drag
// vector. The drag vector is dependent upon the velocity vector so it is not a
// parameter to the argument. Since all of these vectors need to be updated,
// pass in the spaceship as a pointer.
void updateVectors(spaceship_t *spaceship, bool applyThrustVector) {
  // Find the magnitude of the velocityVect.
  elementSize_t velocityVectMag = linearAlg_calcMag(spaceship->velocityVect);

  printf("This is the value of velocityVect x, y: %lld %lld\n",
         spaceship->velocityVect.x, spaceship->velocityVect.y);
  printf("This is the value of velocityVectMag: %lld\n", velocityVectMag);

  // Generate the drag vector by first normalizing the velocity vector.
  vector2D_t dragVect =
      linearAlg_normVect(spaceship->velocityVect, velocityVectMag);

  elementSize_t dragVectMag = DRAG_MAGNITUDE(velocityVectMag);

  printf("This is the value of dragVectorMag: %lld\n", dragVectMag);

  // Next, invert the sign of both the x and y components of the vectors and
  // multiply them by the magnitude of the drag.
  dragVect.x = -dragVect.x * dragVectMag;
  dragVect.y = -dragVect.y * dragVectMag;

  printf("This is the value of dragVect x, y: %lld, %lld\n", dragVect.x,
         dragVect.y);

  // Update the thrust vector. This will only change if the spaceship has
  // rotated. The thrust vector's direction is contained in
  // spaceship.vectorArr[0].
  vector2D_t directVect =
      linearAlg_normVect(spaceship->vectorArr[0], spaceship->directVectMag);

  printf("This is the value of directVect x, y: %lld, %lld\n", directVect.x,
         directVect.y);

  printf("This is the value of thrustVect x, y before: %lld, %lld\n",
         spaceship->thrustVect.x, spaceship->thrustVect.y);

  // Multiply component parts by the thrust magnitude to finish updating the
  // thrust vector.
  spaceship->thrustVect.x = directVect.x * spaceship->thrustVectMag;
  spaceship->thrustVect.y = directVect.y * spaceship->thrustVectMag;

  printf("This is the value of thrustVect x, y after: %lld, %lld\n",
         spaceship->thrustVect.x, spaceship->thrustVect.y);

  // Add the vectors together to calculate the new velocity vector. Only add the
  // thrust vector components if the bool applyThrustVector is true.
  spaceship->velocityVect.x +=
      (applyThrustVector ? dragVect.x : 0) + spaceship->thrustVect.x;
  spaceship->velocityVect.y +=
      (applyThrustVector ? dragVect.y : 0) + spaceship->thrustVect.y;

  // Calculate the updated center points based upon the new velocityVect.
  spaceship->centerPoint.x += spaceship->velocityVect.x;
  spaceship->centerPoint.y += spaceship->velocityVect.y;
}

// Function to move the spaceship forward in the direction it is facing if the
// rocketsAreFiring parameter is true. The spaceship will continue to coast for
// a time after the rockets are turned off (with rocketsAreFiring parameter
// being false). This is more or less a wrapper for the updateVectors function.
void spaceship_translateShip(bool rocketsAreFiring) {
  updateVectors(&spaceship, rocketsAreFiring);
}

// Return a list of x, y coordinates of the spaceship's centerpoint and
// verticies.
coordinates_t *spaceship_getPrinciplePoints() {
  coordinates_t coordinatesArr[NUM_VERTICIES + 1];

  // Initialize the coordinatesArr using spaceship.vectorArr
  for (uint8_t i = 0; i < NUM_VERTICIES + 1; i++) {
    if (i < NUM_VERTICIES) {
      coordinatesArr[i] = (coordinates_t){
          .x = (coordMem_t)(spaceship.vectorArr[i].x + spaceship.centerPoint.x),
          .y =
              (coordMem_t)(spaceship.vectorArr[i].y + spaceship.centerPoint.y)};
    } else { // Append the centerpoint at the end.
      coordinatesArr[i] =
          (coordinates_t){.x = (coordMem_t)spaceship.centerPoint.x,
                          .y = (coordMem_t)spaceship.centerPoint.y};
    }
  }

  return coordinatesArr;
}