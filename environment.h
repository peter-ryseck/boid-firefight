/******************************************************
 * File:           environment.h
 * Project:        Boid Firefight
 * Author:         Peter Ryseck
 * Date Created:   February 8, 2025
 * Last Updated:   February 8, 2025
 *
 * Description:    Logic for wildfire spread and grid operations
 ******************************************************/

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "constants.h"
#include <stdlib.h>

// Constants for environment behavior
#define SPREAD_PROBABILITY 0.04f
#define RANDOM_IGNITION_PROB 0.03f
#define BURNING_DURATION 100

typedef struct {
    int x;  // X-coordinate of the target
    int y;  // Y-coordinate of the target
} HomeTarget;

typedef struct {
    float xval;
    float yval;
    unsigned int state;  // 0: unburnt, 1: burning, 2: burnt, 3: extinguished
    unsigned int timer;
} Cell;

typedef struct {
    Cell** cells;
    unsigned int rows;
    unsigned int cols;
} Grid;

extern Cell grid[GRID_HEIGHT][GRID_WIDTH];

void InitializeGrid(Grid* grid);
void UpdateGrid(Grid* grid);

#endif // ENVIRONMENT_H
