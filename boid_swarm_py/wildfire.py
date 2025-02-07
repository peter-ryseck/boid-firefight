import pygame
import random

# Initialize Pygame
pygame.init()

# Screen dimensions (grid size)
GRID_WIDTH = 500  # Number of columns
GRID_HEIGHT = 500  # Number of rows
CELL_SIZE = 5  # Smaller pixels

# Colors
WHITE = (255, 255, 255)  # Not burnt
RED = (255, 0, 0)        # Burning
BLACK = (0, 0, 0)        # Burnt

# Burning duration (number of frames a cell stays burning before turning burnt)
BURNING_DURATION = 5

# Random ignition probability
RANDOM_IGNITION_PROB = 0.2  # Chance of a new fire starting at a random location
SPREAD_PROBABILITY = 0.3

# Initialize screen
screen = pygame.display.set_mode((GRID_WIDTH * CELL_SIZE, GRID_HEIGHT * CELL_SIZE))
pygame.display.set_caption("2D Wildfire Simulation with Dynamic Wind")

# Initialize grid with burning timers
grid = [[{"state": 0, "timer": 0} for _ in range(GRID_WIDTH)] for _ in range(GRID_HEIGHT)]

# States:
# 0 = Not Burnt
# 1 = Burning
# 2 = Burnt

# Ignite initial fire at the center
grid[GRID_HEIGHT // 2][GRID_WIDTH // 2]["state"] = 1
grid[GRID_HEIGHT // 2][GRID_WIDTH // 2]["timer"] = BURNING_DURATION


def draw_grid():
    """Draw the grid on the screen."""
    for row in range(GRID_HEIGHT):
        for col in range(GRID_WIDTH):
            cell = grid[row][col]
            if cell["state"] == 0:  # Not burnt
                color = WHITE
            elif cell["state"] == 1:  # Burning
                color = RED
            else:  # Burnt
                color = BLACK
            pygame.draw.rect(screen, color, (col * CELL_SIZE, row * CELL_SIZE, CELL_SIZE, CELL_SIZE))


def update_grid():
    """Update the grid based on fire spread rules."""
    global grid
    new_grid = [[cell.copy() for cell in row] for row in grid]  # Deep copy of the grid
    for row in range(GRID_HEIGHT):
        for col in range(GRID_WIDTH):
            cell = grid[row][col]
            if cell["state"] == 1:  # If cell is burning
                # Decrease the burn timer
                new_grid[row][col]["timer"] -= 1
                if new_grid[row][col]["timer"] <= 0:
                    new_grid[row][col]["state"] = 2  # Turn burnt when timer ends

                # Spread fire to neighbors
                for dr, dc in [(-1, 0), (1, 0), (0, -1), (0, 1)]:  # Top, Bottom, Left, Right
                    nr, nc = row + dr, col + dc
                    if 0 <= nr < GRID_HEIGHT and 0 <= nc < GRID_WIDTH:
                        neighbor = grid[nr][nc]
                        if neighbor["state"] == 0:
                            # Base spread probability
                            spread_chance = SPREAD_PROBABILITY
                            # Spread fire based on probability
                            if random.random() < spread_chance:
                                new_grid[nr][nc]["state"] = 1
                                new_grid[nr][nc]["timer"] = BURNING_DURATION

    # Random ignition of new fires
    if random.random() < RANDOM_IGNITION_PROB:
        random_row = random.randint(0, GRID_HEIGHT - 1)
        random_col = random.randint(0, GRID_WIDTH - 1)
        if new_grid[random_row][random_col]["state"] == 0:  # Only ignite unburnt cells
            new_grid[random_row][random_col]["state"] = 1
            new_grid[random_row][random_col]["timer"] = BURNING_DURATION

    grid = new_grid  # Update the grid


def main():
    """Main loop for the simulation."""
    clock = pygame.time.Clock()
    loop_counter = 0
    update_frequency = 5
    running = True

    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

        # Increment loop counter
        loop_counter += 1

        # Update grid and draw every `update_frequency` loops
        if loop_counter % update_frequency == 0:
            update_grid()
            draw_grid()

        # Update display
        pygame.display.flip()

    pygame.quit()

if __name__ == "__main__":
    main()
