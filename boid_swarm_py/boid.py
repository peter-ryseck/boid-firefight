import pygame
import random
import math

pygame.init()

WIDTH, HEIGHT = 800, 600
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Boids Simulation")

BLACK = (0, 0, 0)
WHITE = (255, 255, 255)

NUM_BOIDS = 200
MAX_SPEED = 5
MAX_FORCE = 0.5
MAX_FORCE_WALL = 0.7
NEIGHBOR_RADIUS = 30
AVOID_RADIUS = 20
TARGET_SPAWN_INTERVAL = 2000  # Spawn a new target every 2000 ms (2 seconds)
MAX_TARGETS = 20  # Maximum number of targets allowed on the map
TARGETS = []  # Two initial targets
SEARCH_RADIUS = 100
HOME_TARGET = pygame.math.Vector2(30, 30)  # Home target in the center of the screen
TARGET_REACHED_RADIUS = 10  # Distance to consider a target reached

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

        align_steering = self.calculate_steering(align_sum, align_total, normalize=True)
        cohesion_steering = self.calculate_steering(cohesion_sum, cohesion_total, subtract_pos=True)
        separation_steering = self.calculate_steering(separation_sum, separation_total, normalize=True)

        return align_steering, cohesion_steering, separation_steering

    def calculate_steering(self, vector_sum, total, normalize=False, subtract_pos=False):
        if total > 0:
            vector_sum /= total
            if subtract_pos:
                vector_sum -= self.position
            if normalize:
                vector_sum = (vector_sum.normalize() * MAX_SPEED)
            vector_sum -= self.velocity
            vector_sum = self.limit_force(vector_sum)
        return vector_sum

    def limit_force(self, force, max_force=MAX_FORCE):
        if force.length() > max_force:
            force.scale_to_length(max_force)
        return force

    def target_behavior(self, target):
        desired = target - self.position
        if desired.length() > 0:
            desired = desired.normalize() * MAX_SPEED
        steering = desired - self.velocity
        steering = self.limit_force(steering, 0.5)
        return steering

    def update(self, boids, targets):
        align_force, cohesion_force, separation_force = self.compute_behaviors(boids)

        # Determine the current target
        if not self.heading_home:
            # Combine forces for all targets
            closest_target = None
            closest_distance = SEARCH_RADIUS
            for target in targets:
                distance = self.position.distance_to(target)
                if distance < closest_distance:
                    closest_distance = distance
                    closest_target = target

            target_force = pygame.math.Vector2(0, 0)
            if closest_target is not None:
                target_force = self.target_behavior(closest_target)

                if closest_distance < TARGET_REACHED_RADIUS:
                    targets.remove(closest_target)
                    self.heading_home = True

        else:
            target_force = self.target_behavior(HOME_TARGET)

            if self.position.distance_to(HOME_TARGET) < TARGET_REACHED_RADIUS:
                self.heading_home = False

        self.velocity += (
            align_force * MAX_FORCE +
            cohesion_force * MAX_FORCE +
            separation_force * MAX_FORCE +
            target_force
        )

        if self.velocity.length() > MAX_SPEED:
            self.velocity.scale_to_length(MAX_SPEED)
        self.position += self.velocity

    def draw(self):
        angle = self.velocity.angle_to(pygame.math.Vector2(1, 0))
        size = 7  # Scale down the size of the boid
        points = [
            (self.position.x + size * math.cos(math.radians(angle)),
             self.position.y + size * math.sin(math.radians(angle))),
            (self.position.x - size * 0.5 * math.cos(math.radians(angle + 140)),
             self.position.y - size * 0.5 * math.sin(math.radians(angle + 140))),
            (self.position.x - size * 0.5 * math.cos(math.radians(angle - 140)),
             self.position.y - size * 0.5 * math.sin(math.radians(angle - 140))),
        ]
        pygame.draw.polygon(screen, WHITE, points)

def main():
    global TARGETS
    clock = pygame.time.Clock()
    boids = [Boid(random.randint(30, 30), random.randint(30, 30)) for _ in range(NUM_BOIDS)]

    # Timer for spawning new targets
    pygame.time.set_timer(pygame.USEREVENT, TARGET_SPAWN_INTERVAL)

    running = True
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
        # Spawn a new target at a random position if there are fewer than MAX_TARGETS
        if len(TARGETS) < MAX_TARGETS:
            new_target = pygame.math.Vector2(
                random.randint(50, WIDTH - 50),  # Avoid edges
                random.randint(50, HEIGHT - 50)
            )
            TARGETS.append(new_target)  # Append the new target

        screen.fill(BLACK)

        # Draw the home target
        pygame.draw.circle(screen, (0, 255, 0), (int(HOME_TARGET.x), int(HOME_TARGET.y)), 10)

        # Draw all targets
        for target in TARGETS:
            pygame.draw.circle(screen, (255, 0, 0), (int(target.x), int(target.y)), 5)

        # Update and draw boids
        for boid in boids:
            boid.edges()
            boid.update(boids, TARGETS)
            boid.draw()

        pygame.display.flip()
        clock.tick(60)

    pygame.quit()


if __name__ == "__main__":
    main()
