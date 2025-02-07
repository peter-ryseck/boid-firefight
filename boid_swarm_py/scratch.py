
# WILDFIRE STUFF
GRID_SCALE = 3
GRID_WIDTH = WIDTH//GRID_SCALE
GRID_HEIGHT = HEIGHT//GRID_SCALE

# Colors
WHITE = (255, 255, 255)  # Not burnt
RED = (255, 0, 0)        # Burning
BLACK = (0, 0, 0)        # Burnt

# Burning duration (number of frames a cell stays burning before turning burnt)
BURNING_DURATION = 5

# Random ignition probability
RANDOM_IGNITION_PROB = 0.2  # Chance of a new fire starting at a random location
SPREAD_PROBABILITY = 0.3

# Initialize grid with burning timers
grid = [[{"state": 0, "timer": 0} for _ in range(GRID_WIDTH)] for _ in range(GRID_HEIGHT)]

# States:
# 0 = Not Burnt
# 1 = Burning
# 2 = Burnt

# Ignite initial fire at the center
grid[GRID_HEIGHT*GRID_SCALE // 2][GRID_WIDTH*GRID_SCALE // 2]["state"] = 1
grid[GRID_HEIGHT*GRID_SCALE // 2][GRID_WIDTH*GRID_SCALE // 2]["timer"] = BURNING_DURATION
