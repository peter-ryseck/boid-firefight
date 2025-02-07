/******************************************************
 * File:           display.h
 * Project:        Boid Swarm
 * Author:         Peter Ryseck
 * Date Created:   January 20, 2025
 * Last Updated:   January 20, 2025
 *
 * Description:    Display and rendering operations
 ******************************************************/

#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL.h>

#define SCREEN_WIDTH 1920*0.75
#define SCREEN_HEIGHT 1080*0.75

void InitDisplay(SDL_Window **window, SDL_Renderer **renderer);
void RenderBoids(SDL_Renderer *renderer, Boid *boids, int numBoids);
void CleanupDisplay(SDL_Window *window, SDL_Renderer *renderer);

#endif
