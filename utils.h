/******************************************************
 * File:           utils.h
 * Project:        Boid Swarm
 * Author:         Peter Ryseck
 * Date Created:   January 20, 2025
 * Last Updated:   January 20, 2025
 *
 * Description:    General utility functions
 ******************************************************/

#ifndef UTILS_H
#define UTILS_H

float GetRandomFloat(float min, float max);
float Distance(Boid* boid1, Boid* boid2);
float LimitVector(float *vx, float *vy, float max);
float Magnitude(float *vx, float *vy, float *mag);
float Normalize(float *vx, float *vy);

#endif
