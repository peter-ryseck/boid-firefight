/******************************************************
 * File:           utils.h
 * Project:        Boid Swarm Firefight
 * Author:         Peter Ryseck
 * Date Created:   February 8, 2025
 * Last Updated:   February 8, 2025
 *
 * Description:    General utility functions
 ******************************************************/

#ifndef UTILS_H
#define UTILS_H

float GetRandomFloat(float min, float max);
float Distance(Boid* boid1, Boid* boid2);
float LimitVector(float *vx, float *vy, float min, float max);
float Magnitude(float vx, float vy, float *mag);
float Normalize(float *vx, float *vy);
float EuclideanDistance(float x1, float y1, float x2, float y2);

#endif
