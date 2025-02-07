/******************************************************
 * File:           boid.c
 * Project:        Boid Swarm
 * Author:         Peter Ryseck
 * Date Created:   January 20, 2025
 * Last Updated:   January 20, 2025
 *
 * Description:    Boid logic and information
 * Compile: gcc -O3 -o boid boid.c utils.c display.c -I/opt/homebrew/include/SDL2 -L/opt/homebrew/lib -L/opt/homebrew/opt/openblas/lib -I/opt/homebrew/opt/openblas/include -lopenblas -lSDL2
 ******************************************************/

#include "boid.h"
#include "utils.h"
#include "stdlib.h"
#include "display.h"
#include <math.h>
#include <stdbool.h>

static Boid* InitializeBoids(const unsigned int numBoids) 
{
    Boid* boids = (Boid*)malloc(numBoids * sizeof(Boid));
    if (boids == NULL)
    {
        return NULL;
    }

    for (unsigned int index = 0; index < numBoids; index++)
    {
        boids[index].posx = GetRandomFloat(SCREEN_WIDTH/2, SCREEN_WIDTH/2);
        boids[index].posy = GetRandomFloat(SCREEN_HEIGHT/2, SCREEN_HEIGHT/2);
        boids[index].velx = GetRandomFloat(0, MAX_SPEED);
        boids[index].vely = GetRandomFloat(0, MAX_SPEED);
    }

    return boids;
}

static void ApplySteering(Boid *boid, SteerForce *vectorSum, unsigned int total, float steerForce, bool normalizeFlag, bool subtractPosFlag)
{
    if (total > 0)
    {
        SteerForce avgSteer = {0, 0};
        avgSteer.x = vectorSum->x / total;
        avgSteer.y = vectorSum->y / total;

        if (subtractPosFlag)
        {
            avgSteer.x -= boid->posx;
            avgSteer.y -= boid->posy;
        }

        if (normalizeFlag)
        {
            LimitVector(&avgSteer.x, &avgSteer.y, MAX_SPEED);
        }

        avgSteer.x -= boid->velx;
        avgSteer.y -= boid->vely;

        LimitVector(&avgSteer.x, &avgSteer.y, steerForce);

        boid->velx += avgSteer.x;
        boid->vely += avgSteer.y;
        LimitVector(&boid->velx, &boid->vely, MAX_SPEED);
    }
}

static void ComputeBehavior(Boid *boid, Boid *boids, const unsigned int numBoids)
{
    SteerForce alignSum = {0, 0};
    SteerForce cohesionSum = {0, 0};
    SteerForce separationSum = {0, 0};
    SteerForce posDiff = {0, 0};
    SteerForce diff = {0, 0};
    unsigned int alignTotal = 0, cohesionTotal = 0, separationTotal = 0;

    for (unsigned int index = 0; index < numBoids; index++)
    {
        if (boid == &boids[index])
        {
            continue;
        }

        posDiff.x = boids[index].posx - boid->posx;
        posDiff.y = boids[index].posy - boid->posy;

        float dist = Distance(boid, &boids[index]);
        
        if (dist < ALIGNMENT_RADIUS)
        {
            alignSum.x += boids[index].velx;
            alignSum.y += boids[index].vely;
            alignTotal += 1;
        }

        if (dist < COHESION_RADIUS)
        {
            cohesionSum.x += boids[index].posx;
            cohesionSum.y += boids[index].posy;
            cohesionTotal += 1;
        }

        if (dist < SEPARATION_RADIUS && dist != 0)
        {
            diff.x = -posDiff.x / dist;
            diff.y = -posDiff.y / dist;
            separationSum.x += diff.x;
            separationSum.y += diff.y;
            separationTotal += 1;
        }
    }

    ApplySteering(boid, &alignSum, alignTotal, MAX_ALIGNMENT_FORCE, true, false);
    ApplySteering(boid, &cohesionSum, cohesionTotal, MAX_COHESION_FORCE, false, true);
    ApplySteering(boid, &separationSum, separationTotal, MAX_SEPERATION_FORCE, true, false);
}

static void UpdateBoid(Boid *boid, Boid *boids, const unsigned int numBoids)
{
    ComputeBehavior(boid, boids, numBoids);

    float velMag;
    Magnitude(&boid->velx, &boid->vely, &velMag);
    if (velMag < MIN_SPEED) {
        Normalize(&boid->velx, &boid->vely);
        boid->velx *= MIN_SPEED;
        boid->vely *= MIN_SPEED;
    }

    boid->posx += boid->velx;
    boid->posy += boid->vely;
}

static void Edges(Boid *boid)
{
    SteerForce edgeSum = {0, 0};
    if (boid->posx < WALL_MARGIN)
    {
        edgeSum.x = MAX_SPEED;
    }
    else if (boid->posx > SCREEN_WIDTH - WALL_MARGIN)
    {
        edgeSum.x = -MAX_SPEED;
    }

    if (boid->posy < WALL_MARGIN)
    {
        edgeSum.y = MAX_SPEED;
    }
    else if (boid->posy > SCREEN_HEIGHT - WALL_MARGIN)
    {
        edgeSum.y = -MAX_SPEED;
    }

    float mag;
    Magnitude(&edgeSum.x, &edgeSum.y, &mag);
    if (mag > 0)
    {
        Normalize(&edgeSum.x, &edgeSum.y);
        edgeSum.x = edgeSum.x * MAX_SPEED - boid->velx;
        edgeSum.y = edgeSum.y * MAX_SPEED - boid->vely;
        LimitVector(&edgeSum.x, &edgeSum.y, MAX_WALL_FORCE);
    }
    
    boid->velx += edgeSum.x;
    boid->vely += edgeSum.y;
}

int main()
{
    srand(time(NULL));
    const unsigned int numBoids = 2000;
    Boid* boids = InitializeBoids(numBoids);

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    InitDisplay(&window, &renderer);

    SDL_Event event;
    bool isRunning = true;
    while (isRunning)
    {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = 0;
            }
        }

        for (unsigned int index = 0; index < numBoids; index++)
        {
            Edges(&boids[index]);
            UpdateBoid(&boids[index], boids, numBoids);
        }
        RenderBoids(renderer, boids, numBoids);
    }

    CleanupDisplay(window, renderer);

    free(boids);
    
    return 0;
}
