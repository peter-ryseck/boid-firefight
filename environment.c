/******************************************************
 * File:           environment.c
 * Project:        Boid Firefight
 * Author:         Peter Ryseck
 * Date Created:   January 20, 2025
 * Last Updated:   January 20, 2025
 *
 * Description:    Logic for wildfire spread and grid operations
 ******************************************************/

#include "environment.h"
#include "boid.h"

// Function to initialize grid
void InitializeGrid(Grid* grid) {
    grid->rows = GRID_HEIGHT;
    grid->cols = GRID_WIDTH;

    grid->cells = (Cell**)malloc(grid->rows * sizeof(Cell*));
    for (unsigned int rowIndex = 0; rowIndex < grid->rows; rowIndex++) {
        grid->cells[rowIndex] = (Cell*)malloc(grid->cols * sizeof(Cell));
        for (unsigned int colIndex = 0; colIndex < grid->cols; colIndex++) {
            grid->cells[rowIndex][colIndex].state = 0;  // Set to unburnt initially
            grid->cells[rowIndex][colIndex].timer = 0;
        }
    }
}

Grid* CopyGrid(Grid* grid) 
{
    Grid* newGrid = (Grid*)malloc(sizeof(Grid));
    newGrid->rows = grid->rows;
    newGrid->cols = grid->cols;

    newGrid->cells = (Cell**)malloc(grid->rows * sizeof(Cell*));
    for (unsigned int rowIndex = 0; rowIndex < grid->rows; rowIndex++)
    {
        newGrid->cells[rowIndex] = (Cell*)malloc(grid->cols * sizeof(Cell));
        for (unsigned int colIndex = 0; colIndex < grid->cols; colIndex++)
        {
            newGrid->cells[rowIndex][colIndex] = grid->cells[rowIndex][colIndex];
        }
    }

    return newGrid;
}

void UpdateGrid(Grid* grid) {
    Grid* newGrid = CopyGrid(grid);
    
    for (unsigned int rowIndex = 0; rowIndex < grid->rows; rowIndex++)
    {
        for (unsigned int colIndex = 0; colIndex < grid->cols; colIndex++)
        {
            Cell* cell = &grid->cells[rowIndex][colIndex];

            // If cell is burning
            if (cell->state == 1) 
            {
                newGrid->cells[rowIndex][colIndex].timer -= 1;
                if (newGrid->cells[rowIndex][colIndex].timer <= 0)
                {
                    newGrid->cells[rowIndex][colIndex].state = 2;  // change to burnt
                }

                int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
                for (unsigned int dirIndex = 0; dirIndex < 4; dirIndex++)
                {
                    int newRow = rowIndex + directions[dirIndex][0];
                    int newCol = colIndex + directions[dirIndex][1];
                    if (newRow >= 0 && newRow < grid->rows && newCol >= 0 && newCol < grid->cols)
                    {
                        Cell* neighbor = &grid->cells[newRow][newCol];
                        if (neighbor->state == 0)  // check if not burnt
                        {
                            if ((float)rand() / RAND_MAX < SPREAD_PROBABILITY)
                            {
                                newGrid->cells[newRow][newCol].state = 1;  // change to burning
                                newGrid->cells[newRow][newCol].timer = BURNING_DURATION;
                            }
                        }
                    }
                }
            }
        }
    }
    
    if ((float)rand() / RAND_MAX < RANDOM_IGNITION_PROB)
    {
        unsigned int randomRow = rand() % (grid->rows - 10) + 5; // avoid edges
        unsigned int randomCol = rand() % (grid->cols - 10) + 5;
        if (newGrid->cells[randomRow][randomCol].state == 0)
        {
            newGrid->cells[randomRow][randomCol].state = 1;  // change to burning
            newGrid->cells[randomRow][randomCol].timer = BURNING_DURATION;
        }
    }

    for (unsigned int rowIndex = 0; rowIndex < grid->rows; rowIndex++)
    {
        for (unsigned colIndex = 0; colIndex < grid->cols; colIndex++)
        {
            grid->cells[rowIndex][colIndex] = newGrid->cells[rowIndex][colIndex];
        }
    }

    for (unsigned int index = 0; index < newGrid->rows; index++)
    {
        free(newGrid->cells[index]);
    }
    free(newGrid->cells);
    free(newGrid);
}

void CalculateSectionIntensity(float** sectionIntensity, Grid* grid, Boid* boids, unsigned int numBoids, 
                               unsigned int numSectionsX, unsigned int numSectionsY) 
{
    int sectionWidth = grid->cols / numSectionsX;
    int sectionHeight = grid->rows / numSectionsY;

    for (unsigned int sectionX = 0; sectionX < numSectionsX; sectionX++) {
        for (unsigned int sectionY = 0; sectionY < numSectionsY; sectionY++) {
            int startRow = sectionY * sectionHeight;
            int endRow = (sectionY + 1) * sectionHeight - 1;
            int startCol = sectionX * sectionWidth;
            int endCol = (sectionX + 1) * sectionWidth - 1;

            // Calculate intensity for this section
            float intensity = 0.0f;
            unsigned int activeBoidCount = 0;

            // Count burning cells
            for (int rowIndex = startRow; rowIndex <= endRow; rowIndex++) {
                for (int colIndex = startCol; colIndex <= endCol; colIndex++) {
                    if (rowIndex >= 0 && rowIndex < grid->rows && colIndex >= 0 && colIndex < grid->cols) {
                        Cell* cell = &grid->cells[rowIndex][colIndex];
                        if (cell->state == 1) { // Burning state
                            intensity += 1.0f;
                        }
                    }
                }
            }

            // Count active boids in the region
            for (unsigned int i = 0; i < numBoids; i++) {
                Boid* boid = &boids[i];
                if (!boid->headingHome) { // Only count boids not heading home
                    int boidRow = (int)(boid->posy / CELL_SIZE);
                    int boidCol = (int)(boid->posx / CELL_SIZE);

                    if (boidRow >= startRow && boidRow <= endRow && boidCol >= startCol && boidCol <= endCol) {
                        activeBoidCount++;
                    }
                }
            }

            // Store intensity in the section's index
            sectionIntensity[sectionX][sectionY] = intensity;

            // Ensure intensity is non-negative
            if (sectionIntensity[sectionX][sectionY] < 0.0f) {
                sectionIntensity[sectionX][sectionY] = 0.0f;
            }
        }
    }
}
