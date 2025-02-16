/******************************************************
 * File:           boid.h
 * Project:        Boid Swarm Firefight
 * Author:         Peter Ryseck
 * Date Created:   February 8, 2025
 * Last Updated:   February 8, 2025
 *
 * Description:    Boid logic
 ******************************************************/

#ifndef BOID_H
#define BOID_H

#include <assert.h>
#include <stdbool.h>

typedef struct {
    float posx, posy;
    float velx, vely;
    float energy;
    bool headingHome;
    bool headingHomeToBeRemoved;
} Boid;

typedef struct {
    float x, y;
} SteerForce;

#endif
