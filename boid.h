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

typedef struct {
    float posx, posy;
    float velx, vely;
} Boid;

typedef struct {
    float x, y;
} SteerForce;

// Constants for boid behavior
#define SEPARATION_RADIUS 12.0f
#define ALIGNMENT_RADIUS 30.0f
#define COHESION_RADIUS 30.0f
#define MAX_SEPERATION_FORCE 0.02f
#define MAX_ALIGNMENT_FORCE 0.004f
#define MAX_COHESION_FORCE 0.004f
#define MAX_WALL_FORCE 0.03f
#define WALL_MARGIN 100
#define MAX_SPEED 1.0f
#define MIN_SPEED 0.5f

#endif
