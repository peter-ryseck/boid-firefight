import pygame
import random
import math

pygame.init()

WIDTH, HEIGHT = 800, 600
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Wildfire Extinguishing Boids")

WHITE = (255, 255, 255)  # Not burnt
RED = (255, 0, 0)        # Burning
BLACK = (0, 0, 0)        # Burnt
BLUE = (0, 0, 255)       # Extinguished
BLUE_LIGHT = (0, 150, 255)       # Extinguished

# Boid parameters
NUM_BOIDS = 50
MAX_SPEED = 5
MAX_FORCE = 0.4
MAX_FORCE_TARGET = 1.0
MAX_FORCE_WALL = 0.8
NEIGHBOR_RADIUS = 30
AVOID_RADIUS = 20
MAX_TARGETS = 20  # Maximum number of targets allowed on the map
TARGETS = []  # Two initial targets
SEARCH_RADIUS = 150
HOME_TARGET = pygame.math.Vector2(30, 30)  # Home target in the center of the screen
TARGET_REACHED_RADIUS = 10  # Distance to consider a target reached

# Fire parameters
BURNING_DURATION = 60
RANDOM_IGNITION_PROB = 0.015
SPREAD_PROBABILITY = 0.015

# Grid dimensions (scaled to screen size)
CELL_SIZE = 10
GRID_WIDTH = WIDTH // CELL_SIZE
GRID_HEIGHT = HEIGHT // CELL_SIZE

# Initialize grid
grid = [[{"state": 0, "timer": 0} for _ in range(GRID_WIDTH)] for _ in range(GRID_HEIGHT)]

# Ignite initial fire at the center
grid[GRID_HEIGHT // 2][GRID_WIDTH // 2]["state"] = 1
grid[GRID_HEIGHT // 2][GRID_WIDTH // 2]["timer"] = BURNING_DURATION


class Boid:
    def __init__(self, x, y):
        self.position = pygame.math.Vector2(x, y)
        self.velocity = pygame.math.Vector2(random.uniform(0, 2), random.uniform(0, 2))
        self.heading_home = False  # State: True if heading to the home target

    def edges(self):
        margin = 50
        steer_force = pygame.math.Vector2(0, 0)

        if self.position.x < margin:
            steer_force.x = MAX_SPEED
        elif self.position.x > WIDTH - margin:
            steer_force.x = -MAX_SPEED
        if self.position.y < margin:
            steer_force.y = MAX_SPEED
        elif self.position.y > HEIGHT - margin:
            steer_force.y = -MAX_SPEED

        if steer_force.length() > 0:
            steer_force = (steer_force.normalize() * MAX_SPEED) - self.velocity
            steer_force = self.limit_force(steer_force, MAX_FORCE_WALL)

        self.velocity += steer_force

    def compute_behaviors(self, boids):
        align_sum = pygame.math.Vector2(0, 0)
        cohesion_sum = pygame.math.Vector2(0, 0)
        separation_sum = pygame.math.Vector2(0, 0)

        align_total = cohesion_total = separation_total = 0

        for boid in boids:
            if boid == self:
                continue

            distance = self.position.distance_to(boid.position)

            if distance < NEIGHBOR_RADIUS:
                align_sum += boid.velocity
                cohesion_sum += boid.position
                align_total += 1
                cohesion_total += 1

            if distance < AVOID_RADIUS and distance != 0:
                diff = self.position - boid.position
                diff /= distance
                separation_sum += diff
                separation_total += 1

        align_steering = self.calculate_steering(align_sum, align_total, normalize=True, force=MAX_FORCE*1.0)
        cohesion_steering = self.calculate_steering(cohesion_sum, cohesion_total, subtract_pos=True, force=MAX_FORCE*.5)
        separation_steering = self.calculate_steering(separation_sum, separation_total, normalize=True, force=MAX_FORCE*1.2)

        return align_steering, cohesion_steering, separation_steering

    def calculate_steering(self, vector_sum, total, normalize=False, subtract_pos=False, force=MAX_FORCE):
        if total > 0:
            vector_sum /= total
            if subtract_pos:
                vector_sum -= self.position
            if normalize:
                vector_sum = (vector_sum.normalize() * MAX_SPEED)
            vector_sum -= self.velocity
            vector_sum = self.limit_force(vector_sum, force)
        return vector_sum

    def limit_force(self, force, max_force=MAX_FORCE):
        if force.length() > max_force:
            force.scale_to_length(max_force)
        return force

    def target_behavior(self, target, force=MAX_FORCE_TARGET):
        desired = target - self.position
        if desired.length() > 0:
            desired = desired.normalize() * MAX_SPEED
        steering = desired - self.velocity
        steering = self.limit_force(steering, MAX_FORCE_TARGET)
        return steering

    def update(self, boids, grid):
        align_force, cohesion_force, separation_force = self.compute_behaviors(boids)

        # Initialize target_force to avoid UnboundLocalError
        target_force = pygame.math.Vector2(0, 0)

        # Determine the current target
        if not self.heading_home:
            # Combine forces for all targets
            closest_fire = None
            closest_distance = SEARCH_RADIUS

            # Find the closest target and set it as the target
            for row in range(GRID_HEIGHT):
                for col in range(GRID_WIDTH):
                    cell = grid[row][col]
                    if cell["state"] == 1:
                        cell_center = pygame.math.Vector2(col * CELL_SIZE + CELL_SIZE / 2,
                                                           row * CELL_SIZE + CELL_SIZE / 2)
                        distance = self.position.distance_to(cell_center)
                        if distance < closest_distance:
                            closest_distance = distance
                            closest_fire = cell_center

            if closest_fire is not None:
                target_force = self.target_behavior(closest_fire, MAX_FORCE_TARGET)

                # Extinguish fire if near the target
                if self.position.distance_to(closest_fire) < TARGET_REACHED_RADIUS:
                    col, row = int(closest_fire.x // CELL_SIZE), int(closest_fire.y // CELL_SIZE)
                    if 0 <= row < GRID_HEIGHT and 0 <= col < GRID_WIDTH and grid[row][col]["state"] == 1:
                        grid[row][col]["state"] = 3  # Extinguished
                        self.heading_home = True

        else:
            target_force = self.target_behavior(HOME_TARGET)

            if self.position.distance_to(HOME_TARGET) < TARGET_REACHED_RADIUS:
                self.heading_home = False

        self.velocity += (
            align_force * MAX_FORCE +
            cohesion_force * MAX_FORCE +
            separation_force * MAX_FORCE * 3 +
            target_force
        )

        if self.velocity.length() > MAX_SPEED:
            self.velocity.scale_to_length(MAX_SPEED)
        self.position += self.velocity

    # def draw(self):
    #     angle = self.velocity.angle_to(pygame.math.Vector2(1, 0))
    #     size = 7  # Scale down the size of the boid
    #     points = [
    #         (self.position.x + size * math.cos(angle),
    #          self.position.y + size * math.sin(angle)),
    #         (self.position.x - size * 0.5 * math.cos(angle + 140),
    #          self.position.y - size * 0.5 * math.sin(angle + 140)),
    #         (self.position.x - size * 0.5 * math.cos(angle - 140),
    #          self.position.y - size * 0.5 * math.sin(angle - 140)),
    #     ]
    #     pygame.draw.polygon(screen, BLUE, points)
    def draw(self):
        size = 5  # Radius of the circle representing the boid
        search_radius_color = (0, 0, 255, 50)  # Semi-transparent blue (alpha not used by pygame directly)

        # # Draw the search radius
        # pygame.draw.circle(screen, (0, 0, 255), (int(self.position.x), int(self.position.y)), SEARCH_RADIUS, 1)

        # Draw the boid
        pygame.draw.circle(screen, BLUE, (int(self.position.x), int(self.position.y)), size)


def draw_grid():
    # Draw the grid on the screen
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
                color = BLUE_LIGHT
            pygame.draw.rect(screen, color, (col * CELL_SIZE, row * CELL_SIZE, CELL_SIZE, CELL_SIZE))


def update_grid():
    """Update the grid based on fire spread rules."""
    global grid  # Fix this
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
        random_row = random.randint(3, GRID_HEIGHT - 3)
        random_col = random.randint(3, GRID_WIDTH - 3)
        if new_grid[random_row][random_col]["state"] == 0:  # Only ignite unburnt cells
            new_grid[random_row][random_col]["state"] = 1
            new_grid[random_row][random_col]["timer"] = BURNING_DURATION

    grid = new_grid  # Update the grid


def main():
    clock = pygame.time.Clock()
    boids = [Boid(random.randint(0, 200), random.randint(0, 200)) for _ in range(NUM_BOIDS)]

    # # Timer for spawning new targets
    # pygame.time.set_timer(pygame.USEREVENT, TARGET_SPAWN_INTERVAL)

    running = True
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

        global grid
        update_grid()

        screen.fill(BLACK)
        draw_grid()
        # Update and draw boids
        for boid in boids:
            boid.edges()
            boid.update(boids, grid)
            boid.draw()

        pygame.draw.circle(screen, (0, 255, 0), (int(HOME_TARGET.x), int(HOME_TARGET.y)), 10)

        pygame.display.flip()
        clock.tick(60)

    pygame.quit()


if __name__ == "__main__":
    main()
