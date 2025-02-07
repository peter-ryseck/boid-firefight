import pygame
import random
import math

# Initialize Pygame
pygame.init()

# Screen dimensions for boids simulation
WIDTH, HEIGHT = 800, 600
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Wildfire Extinguishing Boids")

# Grid dimensions (scaled to screen size)
CELL_SIZE = 10
GRID_WIDTH = WIDTH // CELL_SIZE
GRID_HEIGHT = HEIGHT // CELL_SIZE

# Colors
WHITE = (255, 255, 255)  # Not burnt
RED = (255, 0, 0)        # Burning
BLACK = (0, 0, 0)        # Burnt
BLUE = (0, 0, 255)       # Extinguished

# Fire parameters
BURNING_DURATION = 10
RANDOM_IGNITION_PROB = 0.01
SPREAD_PROBABILITY = 0.1

# Boid parameters
NUM_BOIDS = 50
MAX_SPEED = 5
MAX_FORCE = 0.5
SEARCH_RADIUS = 100
TARGET_REACHED_RADIUS = 10

# Initialize grid
grid = [[{"state": 0, "timer": 0} for _ in range(GRID_WIDTH)] for _ in range(GRID_HEIGHT)]

# Ignite initial fire at the center
grid[GRID_HEIGHT // 2][GRID_WIDTH // 2]["state"] = 1
grid[GRID_HEIGHT // 2][GRID_WIDTH // 2]["timer"] = BURNING_DURATION


class Boid:
    def __init__(self, x, y):
        self.position = pygame.math.Vector2(x, y)
        self.velocity = pygame.math.Vector2(random.uniform(-2, 2), random.uniform(-2, 2))

    def target_behavior(self, target):
        desired = target - self.position
        if desired.length() > 0:
            desired = desired.normalize() * MAX_SPEED
        steering = desired - self.velocity
        return self.limit_force(steering)

    def limit_force(self, force, max_force=MAX_FORCE):
        if force.length() > max_force:
            force.scale_to_length(max_force)
        return force

    def update(self, grid):
        # Find the nearest burning cell
        closest_fire = None
        closest_distance = SEARCH_RADIUS

        for row in range(GRID_HEIGHT):
            for col in range(GRID_WIDTH):
                cell = grid[row][col]
                if cell["state"] == 1:  # Burning
                    cell_center = pygame.math.Vector2(col * CELL_SIZE + CELL_SIZE / 2,
                                                       row * CELL_SIZE + CELL_SIZE / 2)
                    distance = self.position.distance_to(cell_center)
                    if distance < closest_distance:
                        closest_distance = distance
                        closest_fire = (row, col, cell_center)

        # Move toward the closest burning cell
        if closest_fire:
            row, col, fire_position = closest_fire
            steering = self.target_behavior(fire_position)
            self.velocity += steering
            if self.velocity.length() > MAX_SPEED:
                self.velocity.scale_to_length(MAX_SPEED)
            self.position += self.velocity

            # Extinguish fire if close enough
            if self.position.distance_to(fire_position) < TARGET_REACHED_RADIUS:
                grid[row][col]["state"] = 3  # Extinguished
        else:
            # Wander if no fire is nearby
            self.position += self.velocity

    def draw(self):
        angle = self.velocity.angle_to(pygame.math.Vector2(1, 0))
        size = 10
        points = [
            (self.position.x + size * math.cos(math.radians(angle)),
             self.position.y + size * math.sin(math.radians(angle))),
            (self.position.x - size * 0.5 * math.cos(math.radians(angle + 140)),
             self.position.y - size * 0.5 * math.sin(math.radians(angle + 140))),
            (self.position.x - size * 0.5 * math.cos(math.radians(angle - 140)),
             self.position.y - size * math.sin(math.radians(angle - 140))),
        ]
        pygame.draw.polygon(screen, BLUE, points)

def draw_grid():
    for row in range(GRID_HEIGHT):
        for col in range(GRID_WIDTH):
            cell = grid[row][col]
            if cell["state"] == 0:  # Not burnt
                color = WHITE
            elif cell["state"] == 1:  # Burning
                color = RED
            elif cell["state"] == 2:  # Burnt
                color = BLACK
            elif cell["state"] == 3:  # Extinguished
                color = BLUE
            pygame.draw.rect(screen, color, (col * CELL_SIZE, row * CELL_SIZE, CELL_SIZE, CELL_SIZE))


def update_grid():
    new_grid = [[cell.copy() for cell in row] for row in grid]
    for row in range(GRID_HEIGHT):
        for col in range(GRID_WIDTH):
            cell = grid[row][col]
            if cell["state"] == 1:  # Burning
                # Decrease burn timer
                new_grid[row][col]["timer"] -= 1
                if new_grid[row][col]["timer"] <= 0:
                    new_grid[row][col]["state"] = 2  # Turn to burnt

                # Spread fire to neighbors
                for dr, dc in [(-1, 0), (1, 0), (0, -1), (0, 1)]:
                    nr, nc = row + dr, col + dc
                    if 0 <= nr < GRID_HEIGHT and 0 <= nc < GRID_WIDTH:
                        neighbor = grid[nr][nc]
                        if neighbor["state"] == 0:  # Not burnt
                            if random.random() < SPREAD_PROBABILITY:
                                new_grid[nr][nc]["state"] = 1
                                new_grid[nr][nc]["timer"] = BURNING_DURATION

    # Random ignition of new fires
    if random.random() < RANDOM_IGNITION_PROB:
        random_row = random.randint(0, GRID_HEIGHT - 1)
        random_col = random.randint(0, GRID_WIDTH - 1)
        if new_grid[random_row][random_col]["state"] == 0:  # Only ignite unburnt cells
            new_grid[random_row][random_col]["state"] = 1
            new_grid[random_row][random_col]["timer"] = BURNING_DURATION

    return new_grid


def main():
    clock = pygame.time.Clock()
    boids = [Boid(random.randint(0, WIDTH), random.randint(0, HEIGHT)) for _ in range(NUM_BOIDS)]
    running = True

    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

        # Update grid
        global grid
        grid = update_grid()

        # Update boids
        for boid in boids:
            boid.update(grid)

        # Draw everything
        screen.fill(BLACK)
        draw_grid()
        for boid in boids:
            boid.draw()
        pygame.display.flip()
        clock.tick(60)

    pygame.quit()


if __name__ == "__main__":
    main()
