#include "spaceship.h"
#include "display.h"
#include "linearAlg.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define SPACESHIP_COLOR DISPLAY_WHITE
#define ERASE_COLOR DISPLAY_BLACK

// Definitions for the positioning of the verticies of the spaceship.
#define NUM_VERTICIES 5

// Starting values for the vertecies of the spaceship represented as x, y pairs
// (vector).
#define VERTEX_1_X 0
#define VERTEX_1_Y -10

#define VERTEX_2_X 7
#define VERTEX_2_Y 10

#define VERTEX_3_X 3
#define VERTEX_3_Y 5

#define VERTEX_4_X -3
#define VERTEX_4_Y 5

#define VERTEX_5_X -7
#define VERTEX_5_Y 10

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

// // Set centerPoint vector.
// void setCenterPoint(vector2D_t *centerPoint, uint16_t xVal, uint16_t yVal) {
//   centerPoint->x = xVal;
//   centerPoint->y = yVal;
// }

// Initialize the spaceship with starting values.
void spaceship_init() {
  spaceship.centerPoint.x = CENTER_X;
  spaceship.centerPoint.y = CENTER_Y;

  spaceship.numVerticies = NUM_VERTICIES;

  // Assign all of vertexArr to constant pre-defined values.
  memcpy(spaceship.vertexArr,
         (const vector2D_t[]){(vector2D_t){.x = VERTEX_1_X, .y = VERTEX_1_Y},
                              (vector2D_t){.x = VERTEX_2_X, .y = VERTEX_2_Y},
                              (vector2D_t){.x = VERTEX_3_X, .y = VERTEX_3_Y},
                              (vector2D_t){.x = VERTEX_4_X, .y = VERTEX_4_Y},
                              (vector2D_t){.x = VERTEX_5_X, .y = VERTEX_5_Y}},
         sizeof spaceship.vertexArr);
}

// Use this function to test various parts of the spaceship code.
void spaceship_runTest() {
  for (uint8_t i = 0; i < NUM_VERTICIES; i++) {
    printf("This is the value of vertex%d: (%d, %d)\n", i + 1,
           spaceship.vertexArr[i].x, spaceship.vertexArr[i].y);
  }
}

// Draw the spaceship.
void spaceship_drawShip() {
  // Loop through the spaceship.vertexArr to get the points for the lines that
  // make up the body of the ship. This will result in a closed path.
  for (uint8_t i = 0; i < spaceship.numVerticies; i++) {
    // Use the vertex at index i and the the one at vertex i + 1 to draw the
    // lines as long as i + 1 is within the size of vertexArr.
    if (i < spaceship.numVerticies - 1) {
      display_drawLine(spaceship.centerPoint.x + spaceship.vertexArr[i].x,
                       spaceship.centerPoint.y + spaceship.vertexArr[i].y,
                       spaceship.centerPoint.x + spaceship.vertexArr[i + 1].x,
                       spaceship.centerPoint.y + spaceship.vertexArr[i + 1].y,
                       SPACESHIP_COLOR);
    } else { // Use the 1st index as the second x, y pair for the line if i + 1
             // is equal to spaceship.numVerticies.
      display_drawLine(spaceship.centerPoint.x + spaceship.vertexArr[i].x,
                       spaceship.centerPoint.y + spaceship.vertexArr[i].y,
                       spaceship.centerPoint.x + spaceship.vertexArr[0].x,
                       spaceship.centerPoint.y + spaceship.vertexArr[0].y,
                       SPACESHIP_COLOR);
    }
  }
}
