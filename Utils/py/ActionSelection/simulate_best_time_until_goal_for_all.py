import math
import numpy as np
from matplotlib import pyplot as plt
from matplotlib.patches import Circle
from tools import action as a
from tools import Simulation as Sim
from naoth import math2d as m2d
from tools import tools
from tools import field_info as field
from simulate_best_angle import main as single

"""
    This file simulates the  best angle for all positions on the field by simulation all the steps necessary to 
    score a goal and compares the time for each rotation. The rotation with the shortest time to goal is the best.
    
    -> Currently the resulting plot looks random and not at all what one would expect
        -> Draw the arrow to the mean of the next ball position instead of the robots rotation
        -> Currently in the model sidekicks are almost equal to short kicks
"""


class State:
    def __init__(self):
        self.pose = m2d.Pose2D()
        self.pose.translation = m2d.Vector2(-4000, -2000)
        self.pose.rotation = math.radians(-120)
        self.rotation_vel = 60  # degrees per sec
        self.walking_vel = 200  # mm per sec
        self.ball_position = m2d.Vector2(100.0, 0.0)

        self.obstacle_list = ([])  # is in global coordinates

    def update_pos(self, glob_pos, rotation):
        self.pose.translation = glob_pos
        self.pose.rotation = rotation


def main():
    state = State()

    # run for the whole field
    x_pos = range(-5200, 5300, 250)  # range(-5200, 5300, 250)
    y_pos = range(-3700, 3800, 250)  # range(-3700, 3800, 250)
    xx, yy = np.meshgrid(x_pos, y_pos)
    vx = np.zeros(xx.shape)
    vy = np.zeros(xx.shape)
    f = np.zeros(xx.shape)
    for ix in range(0, len(x_pos)):
        for iy in range(0, len(y_pos)):
            time, angle = single(float(x_pos[ix]), float(y_pos[iy]), state)

            v = m2d.Vector2(100.0, 0.0).rotate(math.radians(angle))
            vx[iy, ix] = v.x
            vy[iy, ix] = v.y

    plt.figure()
    tools.draw_field()
    Q = plt.quiver(xx, yy, vx, vy)
    plt.show()


if __name__ == "__main__":
    main()
