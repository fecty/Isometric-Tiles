#pragma once

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800
#define FPS 60
#define SCREEN_TITLE "Isometric Tiles Experiment"
#define BG_COLOR BLACK

#define AMPLITUDE 32                      // determins the height of each individual tile
#define MAX_AMPLITUDE (AMPLITUDE * 5)     // max height of an individual tile allowed
#define OSCIl_SPEED 2                     // oscillation speed
#define MAX_OSCIL_SPEED (OSCIl_SPEED * 3) // max oscillation speed allowed
#define GRID_SIZE 15                      // GRID_SIZE * GRID_SIZE is the total number of tiles
#define MAX_GRID_SIZE 50                  // max grid size allowed
#define OSCIL_OPTION 3                    // different height functions for an indivdual tile
