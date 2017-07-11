import math
import numpy as np
from matplotlib import pyplot as plt
from naoth import math2d as m2d
from potentialfield_generation.best_angle_by_goaltime import main as simulate_best_angle
from tools import field_info as field
from tools import tools

"""
    This file simulates the  best angle for all positions on the field by simulation all the steps necessary to 
    score a goal and compares the time for each rotation. The rotation with the shortest time to goal is the best.
"""


class State:
    def __init__(self):
        self.pose = m2d.Pose2D()
        self.pose.translation = m2d.Vector2(-4000, -2000)
        self.pose.rotation = math.radians(0)
        self.rotation_vel = 60  # degrees per sec
        self.walking_vel = 200  # mm per sec
        self.ball_position = m2d.Vector2(100.0, 0.0)

        self.obstacle_list = ([])  # is in global coordinates

    def update_pos(self, glob_pos, rotation):
        self.pose.translation = glob_pos
        self.pose.rotation = math.radians(rotation)


def main():
    state = State()

    cell_width = 200
    rotation_step = 30

    plt.figure()
    axes = plt.gca()
    tools.draw_field()

    x_range = range(int(-field.x_length * 0.5) + 2 * cell_width, int(field.x_length * 0.5) - cell_width, 2 * cell_width)
    y_range = range(int(-field.y_length * 0.5) + 2 * cell_width, int(field.y_length * 0.5) - cell_width, 2 * cell_width)

    # run for the whole field
    for x in x_range:  # range(-3000, 1000, 2 * cell_width):
        for y in y_range:  # range(-3000, 1000, 2 * cell_width):
            time, angle = simulate_best_angle(x, y, state, rotation_step)
            if not np.isinf(time):
                print(time)
                v = m2d.Vector2(100.0, 0.0).rotate(math.radians(angle))
                axes.arrow(x, y, v.x, v.y, head_width=100, head_length=100, fc='k', ec='k')

    plt.show()
    #plt.savefig('goal_time.png')


if __name__ == "__main__":
    main()
