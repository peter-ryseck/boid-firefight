/******************************************************
 * File:           boid.c
 * Project:        Boid Swarm
 * Author:         Peter Ryseck
 * Date Created:   January 20, 2025
 * Last Updated:   January 20, 2025
 *
 * Description:    Boid logic and information
 * Compile: gcc -O3 -o boid boid.c utils.c display.c environment.c -I/opt/homebrew/include/SDL2 -L/opt/homebrew/lib -L/opt/homebrew/opt/openblas/lib -I/opt/homebrew/opt/openblas/include -lopenblas -lSDL2
 ******************************************************/

#include "boid.h"
#include "utils.h"
#include "stdlib.h"
#include "display.h"
#include "environment.h"
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
        boids[index].posx = GetRandomFloat(0, SCREEN_WIDTH);
        boids[index].posy = GetRandomFloat(0, SCREEN_HEIGHT);
        boids[index].velx = GetRandomFloat(-MAX_SPEED, MAX_SPEED);
        boids[index].vely = GetRandomFloat(-MAX_SPEED, MAX_SPEED);
        boids[index].headingHome = false;
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

static void TargetBehavior(Boid *boid, float targetX, float targetY, float maxForceTarget)
{
    // Calculate desired vector
    float desiredX = targetX - boid->posx;
    float desiredY = targetY - boid->posy;

    // Compute magnitude of desired vector
    float magDesired;
    Magnitude(&desiredX, &desiredY, &magDesired);

    // Normalize desired vector if magnitude > 0, and scale by MAX_SPEED
    if (magDesired > 0)
    {
        Normalize(&desiredX, &desiredY);
        desiredX *= MAX_SPEED;
        desiredY *= MAX_SPEED;
    }

    // Calculate steering vector: desired - velocity
    float steeringX = desiredX - boid->velx;
    float steeringY = desiredY - boid->vely;

    // Limit steering force to maxForceTarget
    LimitVector(&steeringX, &steeringY, maxForceTarget);

    // Update boid's velocity with the steering force
    boid->velx += steeringX;
    boid->vely += steeringY;
}

static void UpdateBoid(Boid *boid, Boid *boids, const unsigned int numBoids, HomeTarget* homeTargets, Grid *grid,
            const unsigned int numSectionsX, const unsigned int numSectionsY, float** sectionIntensity)
{
    ComputeBehavior(boid, boids, numBoids);

    SteerForce targetForce = {0, 0};

    // Step 1: Find the section with the highest intensity
    float highestWeightedIntensity = -FLT_MAX;
    int targetSectionX = -1, targetSectionY = -1;

    // Loop through each section
    for (unsigned int sectionX = 0; sectionX < numSectionsX; sectionX++)
    {
        for (unsigned int sectionY = 0; sectionY < numSectionsY; sectionY++)
        {
            // Calculate the center of the section
            float sectionCenterX = (sectionX + 0.5f) * (grid->cols / numSectionsX) * CELL_SIZE;
            float sectionCenterY = (sectionY + 0.5f) * (grid->rows / numSectionsY) * CELL_SIZE;

            // Calculate the distance from the boid to the section center
            float dx = sectionCenterX - boid->posx;
            float dy = sectionCenterY - boid->posy;
            float distance;
            Magnitude(&dx, &dy, &distance);

            // Invert the distance to get a weighting factor (closer = higher weight)
            float distanceWeight = (distance > 0.0f) ? (1.0f / distance) : FLT_MAX;

            // Compute the weighted intensity
            float weightedIntensity = sectionIntensity[sectionX][sectionY] * distanceWeight;
            // float weightedIntensity = sectionIntensity[sectionX][sectionY];

            // Find the section with the highest weighted intensity
            if (weightedIntensity > highestWeightedIntensity)
            {
                highestWeightedIntensity = weightedIntensity;
                targetSectionX = sectionX;
                targetSectionY = sectionY;
            }
        }
    }

    if (!boid->headingHome)
    {
        if (targetSectionX >= 0 && targetSectionY >= 0 && highestWeightedIntensity > 0)
        {
            float targetX = (targetSectionX * (GRID_WIDTH / numSectionsX) + (GRID_WIDTH / numSectionsX) / 2) * CELL_SIZE;
            float targetY = (targetSectionY * (GRID_HEIGHT / numSectionsY) + (GRID_HEIGHT / numSectionsY) / 2) * CELL_SIZE;

            TargetBehavior(boid, targetX, targetY, 0.02);
        }

        float closestDistance = SEARCH_RADIUS;
        float closestFireX = -1, closestFireY = -1;

        // find closest fire
        for (int rowIndex = 0; rowIndex < GRID_HEIGHT; rowIndex++)
        {
            for (int colIndex = 0; colIndex < GRID_WIDTH; colIndex++)
            {
                Cell *cell = &grid->cells[rowIndex][colIndex];
                if (cell->state == 1) // Burning state
                {
                    float cellCenterX = colIndex * CELL_SIZE + CELL_SIZE / 2.0f;
                    float cellCenterY = rowIndex * CELL_SIZE + CELL_SIZE / 2.0f;

                    float dx = cellCenterX - boid->posx;
                    float dy = cellCenterY - boid->posy;
                    float distance;
                    Magnitude(&dx, &dy, &distance);

                    if (distance < closestDistance)
                    {
                        closestDistance = distance;
                        closestFireX = cellCenterX;
                        closestFireY = cellCenterY;
                    }
                }
            }
        }

        // If a fire target is found, compute target force
        if (closestFireX >= 0 && closestFireY >= 0)
        {
            TargetBehavior(boid, closestFireX, closestFireY, MAX_FORCE_TARGET);

            // Extinguish fire if near the target
            if (closestDistance < TARGET_REACHED_RADIUS)
            {
                int col = (int)(closestFireX / CELL_SIZE);
                int row = (int)(closestFireY / CELL_SIZE);
                if (row >= 0 && row < GRID_HEIGHT && col >= 0 && col < GRID_WIDTH && grid->cells[row][col].state == 1)
                {
                    grid->cells[row][col].state = 3; // Extinguished
                    boid->headingHome = true;
                }
            }
        }
    }
    else
    {
        // Head towards the closest home target
        float closestDistance = FLT_MAX;
        float closestHomeX = 0, closestHomeY = 0;

        for (int index = 0; index < NUM_HOME_TARGETS; index++)
        {
            float dx = homeTargets[index].x - boid->posx;
            float dy = homeTargets[index].y - boid->posy;
            float distance;
            Magnitude(&dx, &dy, &distance);

            if (distance < closestDistance)
            {
                closestDistance = distance;
                closestHomeX = homeTargets[index].x;
                closestHomeY = homeTargets[index].y;
            }
        }

        TargetBehavior(boid, closestHomeX, closestHomeY, MAX_FORCE_TARGET);

        if (closestDistance < TARGET_REACHED_RADIUS)
        {
            boid->headingHome = false;
        }
    }

    boid->posx += boid->velx * 2.0;
    boid->posy += boid->vely * 2.0;
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
    const unsigned int numBoids = 1000;
    const unsigned int numSectionsX = 10;
    const unsigned int numSectionsY = 10;

    Boid* boids = InitializeBoids(numBoids);

    // Define home targets
    HomeTarget homeTargets[NUM_HOME_TARGETS] = {
        {100, 100},
        {100, 700},
    };

    Grid grid;
    InitializeGrid(&grid);

    // Allocate memory for sectionIntensity
    float **sectionIntensity = (float **)malloc(numSectionsX * sizeof(float *));
    for (unsigned int i = 0; i < numSectionsX; i++)
    {
        sectionIntensity[i] = (float *)malloc(numSectionsY * sizeof(float));
    }

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    InitDisplay(&window, &renderer);

    SDL_Event event;
    bool isRunning = true;

    while (isRunning)
    {
        while (SDL_PollEvent(&event)) 
        {
            if (event.type == SDL_QUIT) 
            {
                isRunning = false;
            }
        }

        UpdateGridAndCalculateIntensity(&grid, sectionIntensity, boids, numBoids,
                                     numSectionsX, numSectionsY);
        RenderGrid(renderer, &grid);
        RenderHomeTargets(renderer, homeTargets, NUM_HOME_TARGETS);
        for (unsigned int index = 0; index < numBoids; index++)
        {
            Edges(&boids[index]);
            UpdateBoid(&boids[index], boids, numBoids, homeTargets, &grid, numSectionsX, numSectionsY, sectionIntensity);
        }

        RenderBoids(renderer, boids, numBoids);
    }

    CleanupDisplay(window, renderer);


    // Free memory for sectionIntensity
    for (unsigned int i = 0; i < numSectionsX; i++)
    {
        free(sectionIntensity[i]);
    }
    free(sectionIntensity);

    free(boids);
    
    return 0;
}
