/******************************************************
 * File:           boid.h
 * Project:        Boid Swarm
 * Author:         Peter Ryseck
 * Date Created:   January 20, 2025
 * Last Updated:   January 20, 2025
 *
 * Description:    Boid logic and information
 ******************************************************/

#ifndef BOID_H
#define BOID_H

#include <assert.h>
#include <stdbool.h>

typedef struct {
    float posx, posy;
    float velx, vely;
    bool headingHome;
} Boid;

typedef struct {
    float x, y;
} SteerForce;

// Constants for boid behavior
#define SEPARATION_RADIUS 10.0f
#define ALIGNMENT_RADIUS 13.0f
#define COHESION_RADIUS 13.0f
#define MAX_SEPERATION_FORCE 0.05f
#define MAX_ALIGNMENT_FORCE 0.00f
#define MAX_COHESION_FORCE 0.00f
#define MAX_FORCE_TARGET 0.07f
#define MAX_WALL_FORCE 0.03f
#define WALL_MARGIN 10
#define MAX_SPEED 1.0f
#define MIN_SPEED 2.0f
#define NUM_HOME_TARGETS 2
#define SEARCH_RADIUS 200
#define TARGET_REACHED_RADIUS 10

#endif
