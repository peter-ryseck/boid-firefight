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
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (unsigned int index = 0; index < numBoids; index++) {
        // Draw each boid as a small dot based on its position
        float angle = atan2(boids[index].vely, boids[index].velx) + M_PI;
        
        // Draw an arrow indicating the direction
        DrawArrow(renderer, boids[index].posx, boids[index].posy, angle, 6.0f); // Adjust arrow length as needed
    }

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
