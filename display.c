/******************************************************
 * File:           display.c
 * Project:        Boid Swarm Firefight
 * Author:         Peter Ryseck
 * Date Created:   February 8, 2025
 * Last Updated:   February 8, 2025
 *
 * Description:    Display and rendering operations
 ******************************************************/

#include "boid.h" // For the Boid struct
#include "display.h"
#include "environment.h"
#include "utils.h"
#include "constants.h"
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
    
    for (unsigned int index = 0; index < numTargets; ++index) {
        int centerX = (int)homeTargets[index].x;
        int centerY = (int)homeTargets[index].y;
        int radius = 10;

        // Draw the circle
        for (int w = 0; w < radius * 2; ++w) {
            for (int h = 0; h < radius * 2; ++h) {
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
    for (unsigned int rowIndex = 0; rowIndex < grid->rows; ++rowIndex) {
        for (unsigned int colIndex = 0; colIndex < grid->cols; ++colIndex) {
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
                    color = (SDL_Color){0, 100, 255, 255}; // Light Blue
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

void DrawArrow(SDL_Renderer *renderer, float centerX, float centerY, float angle, float length, float mag) {
    // Arrow's line coordinates
    float lineEndX = centerX - length * cos(angle);
    float lineEndY = centerY - length * sin(angle);

    // Arrow's line coordinates
    float lineEndXn = centerX - (length-7) * cos(angle);
    float lineEndYn = centerY - (length-7) * sin(angle);

    // Draw the line for the arrow
    SDL_RenderDrawLine(renderer, (int)centerX, (int)centerY, (int)lineEndX, (int)lineEndY);

    // Arrow's line coordinates
    float lineEndX2 = centerX - length*1.3 * cos(angle);
    float lineEndY2 = centerY - length*1.3 * sin(angle);

    // Draw the line for the arrow
    SDL_RenderDrawLine(renderer, (int)centerX, (int)centerY, (int)lineEndX2, (int)lineEndY2);

    // Arrowhead size
    float arrowheadSize = 7.0f;

    // Angle for the arrowhead
    float arrowheadAngle1 = angle - (M_PI / 6)*1.25*(1-mag*.08);
    float arrowheadAngle2 = angle + (M_PI / 6)*1.25*(1-mag*.08);

    // Arrowhead points
    float headX1 = lineEndX + arrowheadSize * cos(arrowheadAngle1);
    float headY1 = lineEndY + arrowheadSize * sin(arrowheadAngle1);

    float headX2 = lineEndX + arrowheadSize * cos(arrowheadAngle2);
    float headY2 = lineEndY + arrowheadSize * sin(arrowheadAngle2);

    // Draw the arrowhead (two lines)
    SDL_RenderDrawLine(renderer, (int)lineEndX, (int)lineEndY, (int)headX1, (int)headY1);
    SDL_RenderDrawLine(renderer, (int)lineEndX, (int)lineEndY, (int)headX2, (int)headY2);

    // Arrowhead point
    float headX3 = lineEndXn + arrowheadSize*.5 * cos(arrowheadAngle1);
    float headY3 = lineEndYn + arrowheadSize*.5 * sin(arrowheadAngle1);

    float headX4 = lineEndXn + arrowheadSize*.5 * cos(arrowheadAngle2);
    float headY4 = lineEndYn + arrowheadSize*.5 * sin(arrowheadAngle2);

    // Draw the arrowhead (two lines)
    SDL_RenderDrawLine(renderer, (int)lineEndXn, (int)lineEndYn, (int)headX3, (int)headY3);
    SDL_RenderDrawLine(renderer, (int)lineEndXn, (int)lineEndYn, (int)headX4, (int)headY4);
}

void RenderBoids(SDL_Renderer *renderer, Boid *boids, int numBoids) {

    for (unsigned int index = 0; index < numBoids; ++index) {
        // Check if the boid is heading home and set the color accordingly
        if (boids[index].headingHome && !boids[index].headingHomeToBeRemoved) {
            SDL_SetRenderDrawColor(renderer, 100, 150, 255, 180);  // Soft light blue
        } else {
            SDL_SetRenderDrawColor(renderer, 50, 50, 200, 255);  // Normal blue
        }

        float mag;
        Magnitude(boids[index].velx, boids[index].vely, &mag);

        // Draw each boid as an arrow based on its position and velocity
        float angle = atan2(boids[index].vely, boids[index].velx) + M_PI;
        DrawArrow(renderer, boids[index].posx, boids[index].posy, angle, 10.0f, mag); // Adjust arrow length as needed
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


