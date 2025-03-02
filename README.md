# Boid Firefight Swarm Simulation

Overview

This project simulates a boid swarm fighting a wildfire. The simulation implements three fundamental steering behaviors, in addition to fire targeting steering:

*   **Alignment:** Boids adjust their velocity to match the average direction of nearby boids.
*   **Cohesion:** Boids move toward the center of mass of their neighbors.
*   **Separation:** Boids steer away from close neighbors to avoid collisions.

The wildfire grows stochastically outward, with both random spread intensity and random new fire start locations.

The simulation is written in C and uses SDL2 for visualization.

## Prerequisites

Ensure you have the following installed:

*   GCC (for compilation)
*   SDL2 (for graphics)
*   OpenBLAS (for optimized vector operations)

On macOS, you can install dependencies using:

```bash
brew install sdl2 openblas
```

## Compiling the Program

Run the following command to compile the project:

```bash
gcc -O3 -o boid boid.c utils.c display.c \
    -I/opt/homebrew/include/SDL2 -L/opt/homebrew/lib \
    -L/opt/homebrew/opt/openblas/lib -I/opt/homebrew/opt/openblas/include \
    -lopenblas -lSDL2
```

## Running the Simulation

Once compiled, start the simulation with:

```sh
./boid
```

This will launch a window displaying a swarm of boids fighting a wildfire.

## Code Structure

The project consists of the following files:
- **boid.c** – Implements boid logic and behaviors (alignment, cohesion, separation).
- **display.c** – Handles rendering using SDL2.
- **environment.c** - Implements the wildfire logic.
- **utils.c** – Utility functions for vector math and random number generation.
- **boid.h, environment.h, display.h, constants.h** – Header files defining structures, preprocessor directives, and function prototypes.

## Boid Behavior Details

Each boid in the simulation follows these rules:
1. **Alignment**: Adjusts velocity to match neighboring boids within a certain radius.
2. **Cohesion**: Moves toward the average position of nearby boids.
3. **Separation**: Avoids getting too close to other boids by steering away from them.
4. **Edge Wrapping**: If a boid reaches the screen boundary, it is steered back inward.
5. **Fire intensity distribution**: Boids spread based on fire intensity in different sections of the map.

The simulation runs in a loop, updating boid positions and rendering them in real-time.

## Customization & Improvements

Constants

The simulation uses the following constants, defined in constants.h:

Display

- **`CELL_SIZE`** – Size of individual cells representing different fire states.
- **`SCREEN_WIDTH`** / SCREEN_HEIGHT – Dimensions of the simulation window.
- **`GRID_WIDTH`** / GRID_HEIGHT – Number of cells in the grid.
- **`CAP_FRAME_TIME`** – Frame time cap (set to 33 ms for ~30 FPS, 0 for unlimited FPS).

Boid Behavior

- **`MAX_SPEED`** – Maximum speed a boid can move.
- **`MIN_SPEED`** – Minimum speed a boid can move.
- **`ALIGNMENT_RADIUS`** – Distance within which boids align with neighbors.
- **`COHESION_RADIUS`** – Distance within which boids group together.
- **`SEPARATION_RADIUS`** – Distance within which boids avoid each other.
- **`MAX_SEPARATION_FORCE`** – Maximum force with which boids separate from each other.
- **`MAX_ALIGNMENT_FORCE`** – Maximum force with which boids align with each other.
- **`MAX_COHESION_FORCE`** – Maximum force with which boids move toward the center of mass.
- **`MAX_WALL_FORCE`** – Maximum force with which boids are pushed away from the walls.
- **`WALL_MARGIN`** – Distance from the edge of the simulation where wall forces begin to take effect.
- **`MAX_FORCE_TARGET`** – Maximum force applied when seeking a target.
- **`NUM_HOME_TARGETS`** – Number of home locations for boids.
- **`SEARCH_RADIUS`** – Search range for targets.
- **`TARGET_REACHED_RADIUS`** – Distance within which a boid considers a target reached.
- **`SPAWN_FACTOR`** – Factor controlling initial boid population.
- **`MAX_ENERGY`** – Maximum energy level a boid can have.
- **`MIN_ENERGY`** – Minimum energy level before a boid has to return to base to 'refuel'.
- **`MIN_BOID_NUM`** – Minimum number of boids.
- **`MAX_BOID_NUM`** – Maximum number of boids.
- **`MAX_FORCE_INTENSITY_DISTRIBUTION`** – Maximum force for boid distribution w.r.t. fire intensity.

Environment Behavior

- **`MIN_SPREAD_PROBABILITY`** – Minimum probability of fire spreading.
- **`MAX_SPREAD_PROBABILITY`** – Maximum probability of fire spreading.
- **`MIN_SPREAD_FREQ_COUNT`** – Minimum frequency at which fire spreads.
- **`MAX_SPREAD_FREQ_COUNT`** – Maximum frequency at which fire spreads.
- **`RANDOM_IGNITION_PROB`** – Probability of spontaneous fire ignition (set to 0 to disable).
- **`BURNING_DURATION`** – Duration (in frames) that a cell remains burning.
- **`FIRE_INTENSITY_BIAS_FACTOR`** – Factor influencing fire intensity bias. Increase this to target fires more.
- **`SPREAD_INTENSITY_BIAS_FACTOR`** – Factor influencing spread intensity bias. Increase this to distribute boids more evenly.

## License

**MIT License** – Free to use, modify, and distribute.

## Author

**Peter Ryseck**  
March 2, 2025
