#include "spaceship.h"
#include "buttons.h"
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

// Definitions for buttons.
#define LEFT_BTN0_MASK 0x1
#define THRUST_BTN1_MASK 0x2
#define RIGHT_BTN2_MASK 0x4
#define FIRE_BTN3_MASK 0x8

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

#define ROTATE_CCW true
#define ROTATE_CW false

// Definitions for translational movement.
#define ACCELERATION                                                           \
  1.0 // Number of units to add to the velocity vector each
      // tick.

#define MAX_VELOCITY                                                           \
  30.0 // This is the maximum amplitude of the velocity vector.
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

// Definitions for screen wrap.
#define SCREEN_WIDTH (display_width())
#define SCREEN_HEIGHT (display_height())
#define EXTRA_SPACE                                                            \
  12 // The maximum dimensions of the spaceship from the center point with some
     // margin.

#define FIRST_INDEX                                                            \
  0 // Use this to specifically say that the direction vector is contained in
    // the first index of a larger array.

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

// Create an enum for the states.
static enum spaceshipControlStates { init_st, play_st } currentState;

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
void drawShip(bool draw);
void rotateShip(bool rotateCCW);
void translateShip(bool moveForward);

// Declare a spaceship_t variable.
spaceship_t spaceship;

// Declare matrix variables to hold the values of the rotation matricies in the
// counter clock-wise (CCW) and clock-wise (CW) directions.
matrix2x2_t rotationCCW;
matrix2x2_t rotationCW;

// Other Variables.
static bool enabled;

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
  // direction.
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
    rotateShip(rotateCCW);
  }

  // Repeatedly perform the rotation to test its functionality.
  // for (uint8_t i = 0; i < 5; i++) {
  while (true) {
    // Draw the spaceship
    drawShip(DRAW_VALUE);

    // Wait a prescribed amount of time.
    utils_msDelay(DELAY_TIME_MS);

    // Erase the previously drawn spaceship.
    drawShip(ERASE_VALUE);

    // Rotate the spaceship CCW.
    rotateShip(rotateCCW);

    // Move the spaceship forward.
    translateShip(fireRockets);
  }
}

// Draw the spaceship if the parameter draw is true. Otherwise erase the ship.
void drawShip(bool draw) {
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
void rotateShip(bool rotateCCW) {
  // If rotateCCW is true, left multiply the vectors in the vectorArr of
  // spaceship by the rotationCCW matrix.
  if (rotateCCW) {
    for (uint8_t i = 0; i < spaceship.numVerticies; i++) {
      linearAlg_matVectMultAx2D(rotationCCW, &(spaceship.vectorArr[i]));
    }
  } else { // Otherwise multiply by the rotationCW matrix.
    for (uint8_t i = 0; i < spaceship.numVerticies; i++) {
      linearAlg_matVectMultAx2D(rotationCW, &(spaceship.vectorArr[i]));
    }
  }
}

// Function to move the spaceship forward in the direction it is facing if the
// moveForward parameter is true. The spaceship will continue to coast for
// a time after the "rockets" are turned off (with moveForward parameter
// being false). Calculates the next position for the rocket based upon the
// calculations of thrust vector (if moveForward is true), velocity vector, and
// drag vector.
void translateShip(bool moveForward) {
  // Find the magnitude of the velocityVect.
  elementSize_t velocityVectMag = linearAlg_calcMag(spaceship.velocityVect);

  // Generate the drag vector by first normalizing the velocity vector.
  vector2D_t dragVect =
      linearAlg_normVect(spaceship.velocityVect, velocityVectMag);

  elementSize_t dragVectMag = DRAG_MAGNITUDE(velocityVectMag);

  // Next, invert the sign of both the x and y components of the vectors and
  // multiply them by the magnitude of the drag.
  dragVect.x = -dragVect.x * dragVectMag;
  dragVect.y = -dragVect.y * dragVectMag;

  // Update the thrust vector. This will only change if the spaceship has
  // rotated. The thrust vector's direction is contained in
  // spaceship.vectorArr[0].
  vector2D_t directVect =
      linearAlg_normVect(spaceship.vectorArr[0], spaceship.directVectMag);

  // Multiply component parts by the thrust magnitude to finish updating the
  // thrust vector.
  spaceship.thrustVect.x = directVect.x * spaceship.thrustVectMag;
  spaceship.thrustVect.y = directVect.y * spaceship.thrustVectMag;

  // Add the vectors together to calculate the new velocity vector. Only add the
  // thrust vector components if the bool moveForward is true.
  spaceship.velocityVect.x +=
      (moveForward ? spaceship.thrustVect.x : 0) + dragVect.x;
  spaceship.velocityVect.y +=
      (moveForward ? spaceship.thrustVect.y : 0) + dragVect.y;

  // Calculate the updated center points based upon the new velocityVect.
  spaceship.centerPoint.x += spaceship.velocityVect.x;
  spaceship.centerPoint.y += spaceship.velocityVect.y;
}

// Function that handles the movement and firing of the ship.
void spaceship_moveShip(bool rotateCCW, bool rotateCW, bool moveForward,
                        bool shoot) {
  // Erase the ship before updating any parameters.
  drawShip(false);

  // Translate the ship if the move forward argument is true. If it is false but
  // it was true in the past the ship should coast for a bit.
  translateShip(moveForward);

  // Code to handle screen wrap.
  if (spaceship.centerPoint.x <= (-EXTRA_SPACE)) {
    spaceship.centerPoint.x = SCREEN_WIDTH + EXTRA_SPACE;
  } else if (spaceship.centerPoint.x >= (SCREEN_WIDTH + EXTRA_SPACE)) {
    spaceship.centerPoint.x = -EXTRA_SPACE;
  }

  if (spaceship.centerPoint.y <= (-EXTRA_SPACE)) {
    spaceship.centerPoint.y = SCREEN_HEIGHT + EXTRA_SPACE;
  } else if (spaceship.centerPoint.y >= (SCREEN_HEIGHT + EXTRA_SPACE)) {
    spaceship.centerPoint.y = -EXTRA_SPACE;
  }

  // Rotate the ship the appropriate direction if rotateCCW xor rotateCW are
  // true.
  if (rotateCCW && !rotateCW) {
    rotateShip(ROTATE_CCW);
  } else if (!rotateCCW && rotateCW) {
    rotateShip(ROTATE_CW);
  }

  // Draw the ship with the new parameters.
  drawShip(true);
}

// Return a list of x, y coordinates of the spaceship's centerpoint and
// verticies.
void spaceship_getPrinciplePoints(coordinates_t *coordinatesArr) {
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
}

// Standard tick function for spaceship.
void spaceship_tick() {
  if (enabled) {
    // asteroid_debugState();
  }
  switch (currentState) {
  case init_st:
    if (!enabled) {
      currentState = init_st;
    } else {
      currentState = play_st;
    }
    break;
  case play_st:
    if (!enabled) {
      drawShip(false);
      currentState = init_st;
    } else {
      bool fire = false;
      bool thrust = false;
      bool turnLeft = false;
      bool turnRight = false;
      if (buttons_read() & FIRE_BTN3_MASK) {
        printf("FIRE BUTTON\n");
        fire = true;
      }
      if (buttons_read() & THRUST_BTN1_MASK) {
        printf("THRUST BUTTON\n");
        thrust = true;
      }
      if ((buttons_read() & LEFT_BTN0_MASK) &&
          (buttons_read() & RIGHT_BTN2_MASK)) {
        printf("LEFT AND RIGHT BUTTONS\n");
      } else if (buttons_read() & LEFT_BTN0_MASK) {
        printf("LEFT BUTTON\n");
        turnLeft = true;
      } else if (buttons_read() & RIGHT_BTN2_MASK) {
        printf("RIGHT BUTTON\n");
        turnRight = true;
      }
      spaceship_moveShip(turnLeft, turnRight, thrust, fire);
      currentState = play_st;
    }
    break;
  default:
    break;
  }
}

// Enable spaceship.
void spaceship_enable() { enabled = true; }

// Disable Spaceship.
void spaceship_disable() {
  drawShip(false);
  enabled = false;
}