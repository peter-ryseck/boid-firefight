/******************************************************
 * File:           constants.h
 * Project:        Boid Swarm Firefight
 * Author:         Peter Ryseck
 * Date Created:   February 8, 2025
 * Last Updated:   February 8, 2025
 *
 * Description:    Macro constants used throughout boid-firefight
 ******************************************************/

#ifndef CONSTANTS_H
#define CONSTANTS_H

// Display
#define CELL_SIZE 6
#define SCREEN_WIDTH 1800
#define SCREEN_HEIGHT 1020
#define GRID_WIDTH (SCREEN_WIDTH / CELL_SIZE)
#define GRID_HEIGHT (SCREEN_HEIGHT / CELL_SIZE)
#define CAP_FRAME_TIME 33 // 33 ms is 30 fps, set to 0 to avoid capping frame rate

// Boid behavior
#define SEPARATION_RADIUS 5.0f
#define ALIGNMENT_RADIUS 17.0f
#define COHESION_RADIUS 17.0f
#define MAX_SEPERATION_FORCE 0.3f
#define MAX_ALIGNMENT_FORCE 0.05f
#define MAX_COHESION_FORCE 0.05f
#define MAX_FORCE_TARGET 0.6f
#define MAX_WALL_FORCE 0.3f
#define WALL_MARGIN 30
#define MAX_SPEED 6.0f
#define MIN_SPEED 2.0f
#define NUM_HOME_TARGETS 4
#define SEARCH_RADIUS 200
#define TARGET_REACHED_RADIUS 10
#define SPAWN_FACTOR 3.0
#define MAX_ENERGY 2000
#define MIN_ENERGY 100
#define MIN_BOID_NUM 200
#define MAX_BOID_NUM 1000
#define MAX_FORCE_INTENSITY_DISTRIBUTION 0.3

// Environment behavior
#define MIN_SPREAD_PROBABILITY 0.02f
#define MAX_SPREAD_PROBABILITY 0.1f
#define MAX_SPREAD_FREQ_COUNT 900
#define MIN_SPREAD_FREQ_COUNT 200
#define RANDOM_IGNITION_PROB 0.007f
#define BURNING_DURATION 50
#define FIRE_INTENSITY_BIAS_FACTOR 5000

#endif // CONSTANTS_H