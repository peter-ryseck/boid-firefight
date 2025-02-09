/******************************************************
 * File:           display.c
 * Project:        Boid Swarm
 * Author:         Peter Ryseck
 * Date Created:   January 20, 2025
 * Last Updated:   January 20, 2025
 *
 * Description:    Display and rendering operations
 ******************************************************/

#include "boid.h" // For the Boid struct
#include "display.h"
#include "environment.h"
#include <stdio.h>

void InitDisplay(SDL_Window **window, SDL_Renderer **renderer) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }

    *window = SDL_CreateWindow(
        "Boid Swarm Simulation",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (!*window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (!*renderer) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(*window);
        SDL_Quit();
        exit(1);
    }
}

void RenderHomeTargets(SDL_Renderer *renderer, HomeTarget *homeTargets, unsigned int numTargets) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Set the color to green
    
    for (unsigned int i = 0; i < numTargets; i++) {
        int centerX = (int)homeTargets[i].x;
        int centerY = (int)homeTargets[i].y;
        int radius = 10;

        // Draw the circle
        for (int w = 0; w < radius * 2; w++) {
            for (int h = 0; h < radius * 2; h++) {
                int dx = radius - w; // Horizontal distance from center
                int dy = radius - h; // Vertical distance from center
                if ((dx * dx + dy * dy) <= (radius * radius)) {
                    SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
                }
            }
        }
    }
}

void RenderGrid(SDL_Renderer *renderer, Grid *grid) {
    for (unsigned int rowIndex = 0; rowIndex < grid->rows; rowIndex++) {
        for (unsigned int colIndex = 0; colIndex < grid->cols; colIndex++) {
            Cell *cell = &grid->cells[rowIndex][colIndex];
            SDL_Color color;

            switch (cell->state) {
                case 0: // Not burnt
                    color = (SDL_Color){255, 255, 255, 255}; // White
                    break;
                case 1: // Burning
                    color = (SDL_Color){255, 0, 0, 255}; // Red
                    break;
                case 2: // Burnt
                    color = (SDL_Color){0, 0, 0, 255}; // Black
                    break;
                case 3: // Extinguished
                    color = (SDL_Color){0, 0, 255, 255}; // Light Blue
                    break;
                default:
                    color = (SDL_Color){255, 255, 255, 255}; // Default to white
                    break;
            }

            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

            // Draw the cell as a filled rectangle
            SDL_Rect rect = {
                .x = colIndex * CELL_SIZE,
                .y = rowIndex * CELL_SIZE,
                .w = CELL_SIZE,
                .h = CELL_SIZE
            };
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

// void RenderBoids(SDL_Renderer *renderer, Boid *boids, int numBoids) {
//     SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

//     SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

//     for (unsigned int index = 0; index < numBoids; index++) {
//         // Draw each boid as a small dot based on its position
//         float angle = atan2(boids[index].vely, boids[index].velx) + M_PI;
        
//         // Draw an arrow indicating the direction
//         DrawArrow(renderer, boids[index].posx, boids[index].posy, angle, 6.0f); // Adjust arrow length as needed
//     }

//     SDL_RenderPresent(renderer);
// }


void DrawArrow(SDL_Renderer *renderer, float centerX, float centerY, float angle, float length) {
    // Arrow's line coordinates
    float lineEndX = centerX + length * cos(angle);
    float lineEndY = centerY + length * sin(angle);

    // Draw the line for the arrow
    SDL_RenderDrawLine(renderer, (int)centerX, (int)centerY, (int)lineEndX, (int)lineEndY);

    // Arrowhead size
    float arrowheadSize = 6.0f;

    // Angle for the arrowhead
    float arrowheadAngle1 = angle - M_PI / 6;
    float arrowheadAngle2 = angle + M_PI / 6;

    // Arrowhead points
    float headX1 = lineEndX + arrowheadSize * cos(arrowheadAngle1);
    float headY1 = lineEndY + arrowheadSize * sin(arrowheadAngle1);

    float headX2 = lineEndX + arrowheadSize * cos(arrowheadAngle2);
    float headY2 = lineEndY + arrowheadSize * sin(arrowheadAngle2);

    // Draw the arrowhead (two lines)
    SDL_RenderDrawLine(renderer, (int)lineEndX, (int)lineEndY, (int)headX1, (int)headY1);
    SDL_RenderDrawLine(renderer, (int)lineEndX, (int)lineEndY, (int)headX2, (int)headY2);
}

void RenderBoids(SDL_Renderer *renderer, Boid *boids, int numBoids) {
    // Clear the screen with a black background

    for (unsigned int index = 0; index < numBoids; index++) {
        // Check if the boid is heading home and set the color accordingly
        if (boids[index].headingHome) {
            // Darker blue when heading home
            SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255);  // Dark Blue (RGB: 0, 0, 139)
        } else {
            // Default color (light blue)
            SDL_SetRenderDrawColor(renderer, 0, 100, 255, 255);  // Light Blue
        }

        // Draw each boid as an arrow based on its position and velocity
        float angle = atan2(boids[index].vely, boids[index].velx) + M_PI;
        DrawArrow(renderer, boids[index].posx, boids[index].posy, angle, 6.0f); // Adjust arrow length as needed
    }

    // Present the rendered frame
    SDL_RenderPresent(renderer);  
}

void CleanupDisplay(SDL_Window *window, SDL_Renderer *renderer) {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}


