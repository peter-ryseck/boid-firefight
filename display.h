/******************************************************
 * File:           display.h
 * Project:        Boid Swarm Firefight
 * Author:         Peter Ryseck
 * Date Created:   February 8, 2025
 * Last Updated:   February 8, 2025
 *
 * Description:    Display and rendering operations
 ******************************************************/

#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL.h>
#include "constants.h"
#include "environment.h"

void InitDisplay(SDL_Window **window, SDL_Renderer **renderer);
void RenderBoids(SDL_Renderer *renderer, Boid *boids, int numBoids);
void RenderGrid(SDL_Renderer *renderer, Grid *grid);
void CleanupDisplay(SDL_Window *window, SDL_Renderer *renderer);
void RenderHomeTargets(SDL_Renderer *renderer, HomeTarget *homeTargets, unsigned int numTargets);

#endif // DISPLAY_H
