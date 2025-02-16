/******************************************************
 * File:           boid.c
 * Project:        Boid Swarm Firefight
 * Author:         Peter Ryseck
 * Date Created:   February 8, 2025
 * Last Updated:   February 8, 2025
 *
 * Description:    Boid logic
 * Compile: gcc -O3 -o boid boid.c utils.c display.c environment.c -I/opt/homebrew/include/SDL2 -L/opt/homebrew/lib -L/opt/homebrew/opt/openblas/lib -I/opt/homebrew/opt/openblas/include -lopenblas -lSDL2
 ******************************************************/

#include "boid.h"
#include "utils.h"
#include "stdlib.h"
#include "display.h"
#include "environment.h"
#include "constants.h"
#include <math.h>
#include <stdbool.h>
#include "constants.h"

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
        boids[index].energy = MAX_ENERGY;
        boids[index].headingHomeToBeRemoved = false;
        boids[index].headingHome = false;
    }

    return boids;
}

static Boid* AddBoid(Boid* boids, unsigned int* numBoids, unsigned int locationX, unsigned int locationY) 
{
    // Increase the size of the boids array by 1
    Boid* newBoids = (Boid*)realloc(boids, (*numBoids + 1) * sizeof(Boid));
    if (newBoids == NULL)
    {
        return NULL; // Return NULL if memory allocation fails
    }

    // Add the new boid at the end of the array
    newBoids[*numBoids].posx = locationX;
    newBoids[*numBoids].posy = locationY;
    newBoids[*numBoids].velx = GetRandomFloat(-MAX_SPEED, MAX_SPEED);
    newBoids[*numBoids].vely = GetRandomFloat(-MAX_SPEED, MAX_SPEED);
    newBoids[*numBoids].energy = MAX_ENERGY;
    newBoids[*numBoids].headingHome = false;

    // Increment the boid count
    (*numBoids)++;

    return newBoids; // Return the updated array
}

static Boid* RemoveBoid(Boid* boids, unsigned int* numBoids, unsigned int indexToRemove)
{
    if (boids[indexToRemove].headingHomeToBeRemoved == true && boids[indexToRemove].headingHome == false)
    {
        // Check if the index is valid
        if (indexToRemove >= *numBoids)
        {
            return boids; // Return the original array if the index is invalid
        }

        // Shift all elements after the index to the left
        for (unsigned int index = indexToRemove; index < *numBoids - 1; index++)
        {
            boids[index] = boids[index + 1];
        }

        // Reduce the size of the array by 1
        Boid* newBoids = (Boid*)realloc(boids, (*numBoids - 1) * sizeof(Boid));
        if (newBoids == NULL && *numBoids - 1 > 0)
        {
            // If realloc fails and the array is not empty, keep the original array
            return boids;
        }

        // Decrement the boid count
        (*numBoids)--;

        return newBoids; // Return the updated array
    }
    else
    {
        return boids;
    }
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
            LimitVector(&avgSteer.x, &avgSteer.y, MIN_SPEED, MAX_SPEED);
        }

        avgSteer.x -= boid->velx;
        avgSteer.y -= boid->vely;

        LimitVector(&avgSteer.x, &avgSteer.y, 0, steerForce);

        boid->velx += avgSteer.x;
        boid->vely += avgSteer.y;
        LimitVector(&boid->velx, &boid->vely, MIN_SPEED, MAX_SPEED);
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
    Magnitude(desiredX, desiredY, &magDesired);

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
    LimitVector(&steeringX, &steeringY, 0, maxForceTarget);

    // Update boid's velocity with the steering force
    boid->velx += steeringX;
    boid->vely += steeringY;
}

static void UpdateBoid(Boid *boid, Boid *boids, unsigned int numBoids, HomeTarget* homeTargets, Grid *grid,
            const unsigned int numSectionsX, const unsigned int numSectionsY, float** sectionIntensity)
{
    ComputeBehavior(boid, boids, numBoids);

    SteerForce targetForce = {0, 0};

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
            float distance = EuclideanDistance(sectionCenterX, sectionCenterY, boid->posx, boid->posy);

            // Limit distance so that boids don't go straight to center of section
            if (distance < 30)
            {
                distance = 30; 
            }

            // Invert the distance to get a weighting factor (closer = higher weight)
            float distanceWeight = (distance > 0.0f) ? (1.0f / distance) : FLT_MAX;

            // Compute the weighted intensity
            float weightedIntensity = sectionIntensity[sectionX][sectionY] * distanceWeight;

            // Find the section with the highest weighted intensity
            if (weightedIntensity > highestWeightedIntensity)
            {
                highestWeightedIntensity = weightedIntensity;
                targetSectionX = sectionX;
                targetSectionY = sectionY;
            }
        }
    }

    if (!boid->headingHome && !boid->headingHomeToBeRemoved && (boid->energy > MIN_ENERGY))
    {
        if (targetSectionX >= 0 && targetSectionY >= 0 && highestWeightedIntensity > 0)
        {
            float targetX = (targetSectionX * (GRID_WIDTH / numSectionsX) + (GRID_WIDTH / numSectionsX) / 2) * CELL_SIZE;
            float targetY = (targetSectionY * (GRID_HEIGHT / numSectionsY) + (GRID_HEIGHT / numSectionsY) / 2) * CELL_SIZE;

            TargetBehavior(boid, targetX, targetY, MAX_FORCE_INTENSITY_DISTRIBUTION);
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

                    float distance = EuclideanDistance(cellCenterX, cellCenterY, boid->posx, boid->posy);

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
            float distance = EuclideanDistance(homeTargets[index].x, homeTargets[index].y, boid->posx, boid->posy);

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
            boid->energy = MAX_ENERGY;
        }
    }

    float mag;
    Magnitude(boid->velx, boid->vely, &mag);
    boid->energy = fmaxf(0.0f, (boid->energy - mag));
    boid->posx += boid->velx ;
    boid->posy += boid->vely ;
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
    Magnitude(edgeSum.x, edgeSum.y, &mag);
    if (mag > 0)
    {
        Normalize(&edgeSum.x, &edgeSum.y);
        edgeSum.x = edgeSum.x * MAX_SPEED - boid->velx;
        edgeSum.y = edgeSum.y * MAX_SPEED - boid->vely;
        LimitVector(&edgeSum.x, &edgeSum.y, 0, MAX_WALL_FORCE);
    }
    
    boid->velx += edgeSum.x;
    boid->vely += edgeSum.y;
}

// Function to handle mouse clicks and update grid
void HandleMouseClick(Grid* grid, int mouseX, int mouseY) {
    int col = mouseX / CELL_SIZE;
    int row = mouseY / CELL_SIZE;

    if (row >= 0 && row < grid->rows && col >= 0 && col < grid->cols) {
        grid->cells[row][col].state = 1;  // Example: Set to burning on click
        printf("Cell at (%d, %d) set to burning\n", row, col);
    }
}

int main()
{
    srand(time(NULL));  // Random seed

    // Set number of boids to start and sections of map
    unsigned int numBoids = MIN_BOID_NUM;
    const unsigned int numSectionsX = 5;
    const unsigned int numSectionsY = 5;

    Boid* boids = InitializeBoids(numBoids);

    // Define home targets
    HomeTarget homeTargets[NUM_HOME_TARGETS] = {
        {200, 100},
        {1600, 100},
        {200, 900},
        {1500, 600},
    };

    Grid grid;
    InitializeGrid(&grid);

    float totalBurning = 0;

    // Initialize spreadProbability and randomness control variables
    float spreadProbability = MIN_SPREAD_PROBABILITY;
    unsigned int updateFrequency = MIN_SPREAD_FREQ_COUNT;
    unsigned int iterationCounter = 0;

    // Allocate memory for sectionIntensity
    float **sectionIntensity = (float **)malloc(numSectionsX * sizeof(float *));
    for (unsigned int index = 0; index < numSectionsX; index++)
    {
        sectionIntensity[index] = (float *)malloc(numSectionsY * sizeof(float));
    }

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    InitDisplay(&window, &renderer);

    SDL_Event event;
    bool isRunning = true;
    bool mouseHeld = false;
    Uint32 lastFireSpawnTime = 0; // Track last fire spawn time

    while (isRunning)
    {
        Uint32 startTime = SDL_GetTicks();  // Start timing the frame

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                isRunning = false;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    mouseHeld = true;  // Mouse is held down
                }
            }
            else if (event.type == SDL_MOUSEBUTTONUP)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    mouseHeld = false;  // Mouse is released
                }
            }
        }

        // Limit fire spawn rate to every 50ms
        if (mouseHeld && SDL_GetTicks() - lastFireSpawnTime > 30)
        {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            int cellX = mouseX / CELL_SIZE;
            int cellY = mouseY / CELL_SIZE;

            if (cellX >= 0 && cellX < grid.cols && cellY >= 0 && cellY < grid.rows)
            {
                grid.cells[cellY][cellX].state = 1;  // Set to burning
                grid.cells[cellY][cellX].timer = BURNING_DURATION;
            }

            lastFireSpawnTime = SDL_GetTicks(); // Update last spawn time
        }

        // Adjust spreadProbability occasionally
        if (++iterationCounter >= updateFrequency)
        {
            spreadProbability = GetRandomFloat(MIN_SPREAD_PROBABILITY, MAX_SPREAD_PROBABILITY);
            updateFrequency = GetRandomFloat(MIN_SPREAD_FREQ_COUNT, MAX_SPREAD_FREQ_COUNT);
            iterationCounter = 0;
        }

        // Add boids if more are needed
        if (totalBurning * SPAWN_FACTOR > numBoids && numBoids < MAX_BOID_NUM)
        {
            for (unsigned int index = 0; index < NUM_HOME_TARGETS; index++)
            {
                boids = AddBoid(boids, &numBoids, homeTargets[index].x, homeTargets[index].y);
            }
        }

        // Remove boids if less are needed
        if ((numBoids > totalBurning * SPAWN_FACTOR) && (numBoids > MIN_BOID_NUM))
        {
            float randIndex = GetRandomFloat(0, numBoids - 1);
            boids[(int)randIndex].headingHome = true;
            boids[(int)randIndex].headingHomeToBeRemoved = true;
        }

        UpdateGridAndCalculateIntensity(&grid, sectionIntensity, boids, numBoids,
                                        numSectionsX, numSectionsY, &totalBurning, spreadProbability);
        RenderGrid(renderer, &grid);
        RenderHomeTargets(renderer, homeTargets, NUM_HOME_TARGETS);
        for (unsigned int index = 0; index < numBoids; index++)
        {
            Edges(&boids[index]);
            UpdateBoid(&boids[index], boids, numBoids, homeTargets, &grid, numSectionsX, numSectionsY, sectionIntensity);
            boids = RemoveBoid(boids, &numBoids, index);
        }

        RenderBoids(renderer, boids, numBoids);

        // Measure frame time
        Uint32 frameTime = SDL_GetTicks() - startTime;

        // Cap frame rate
        if (frameTime < CAP_FRAME_TIME)
        {
            SDL_Delay(CAP_FRAME_TIME - frameTime);
        }
    }

    CleanupDisplay(window, renderer);

    // Free memory
    for (unsigned int index = 0; index < numSectionsX; index++)
    {
        free(sectionIntensity[index]);
    }
    free(sectionIntensity);

    free(boids);

    return 0;
}
