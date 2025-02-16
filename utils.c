/******************************************************
 * File:           utils.c
 * Project:        Boid Swarm Firefight
 * Author:         Peter Ryseck
 * Date Created:   February 8, 2025
 * Last Updated:   February 8, 2025
 *
 * Description:    General utility functions
 ******************************************************/

#include "boid.h"
#include "constants.h"
#include <stdio.h>
#include "stdlib.h"
#include <time.h>
#include <math.h>

float GetRandomFloat(float min, float max)
{
    // Generate a random float between 0.0 and 1.0
    float random = (float)rand() / (float)RAND_MAX;

    // Scale and shift the value to the desired range
    return min + random * (max - min);
}

float EuclideanDistance(float x1, float y1, float x2, float y2)
{
    return sqrtf((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

float Distance(Boid* boid1, Boid* boid2)
{
    return EuclideanDistance(boid1->posx, boid1->posy, boid2->posx, boid2->posy);
}

void Magnitude(float vx, float vy, float *mag)
{
    *mag = EuclideanDistance(0.0f, 0.0f, vx, vy);
}

void LimitVector(float *vx, float *vy, float min, float max)
{
    float mag;
    Magnitude(*vx, *vy, &mag);
    if (mag > max && mag > 0)
    {
        *vx = (*vx / mag) * max;
        *vy = (*vy / mag) * max;
    }
    else if (mag < min && mag > 0)
    {
        *vx = (*vx / mag) * min;
        *vy = (*vy / mag) * min;
    }
}

void Normalize(float *vx, float *vy)
{
    float len;
    Magnitude(*vx, *vy, &len);
    if (len > 0) {
        *vx /= len;
        *vy /= len;
    }
}
