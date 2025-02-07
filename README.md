# Boid Swarm Simulation

Overview

This project simulates a boid swarm, a model for flocking behavior in birds, fish, and other collective animal movement. The simulation implements three fundamental steering behaviors:

*   **Alignment:** Boids adjust their velocity to match the average direction of nearby boids.
*   **Cohesion:** Boids move toward the center of mass of their neighbors.
*   **Separation:** Boids steer away from close neighbors to avoid collisions.

The simulation is written in C and uses SDL2 for visualization.

Compilation & Execution

Prerequisites

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

This will launch a window displaying a swarm of boids moving based on alignment, cohesion, and separation rules.

## Code Structure

The project consists of the following files:
- **boid.c** – Implements boid logic and behaviors (alignment, cohesion, separation).
- **display.c** – Handles rendering using SDL2.
- **utils.c** – Utility functions for vector math and random number generation.
- **boid.h & display.h** – Header files defining structures and function prototypes.

## Boid Behavior Details

Each boid in the simulation follows these rules:
1. **Alignment**: Adjusts velocity to match neighboring boids within a certain radius.
2. **Cohesion**: Moves toward the average position of nearby boids.
3. **Separation**: Avoids getting too close to other boids by steering away from them.
4. **Edge Wrapping**: If a boid reaches the screen boundary, it is steered back inward.

The simulation runs in a loop, updating boid positions and rendering them in real-time.

## Customization & Improvements

You can tweak the following parameters in `boid.h` to modify the simulation:

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

## License

**MIT License** – Free to use, modify, and distribute.

## Author

**Peter Ryseck**  
January 20, 2025
