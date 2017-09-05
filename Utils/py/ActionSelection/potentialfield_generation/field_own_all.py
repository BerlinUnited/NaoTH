import os
import math
import pickle
import numpy as np
from matplotlib import pyplot as plt

from naoth import math2d as m2d
from potentialfield_generation.field_own import main as simulate_best_angle
from tools import field_info as field
from tools import tools

"""
This file simulates the  best angle for all positions on the field by simulation all the steps necessary to 
score a goal and compares the time for each rotation. The rotation with the shortest time to goal is the best.
    
Example:
    run without any parameters

        $ python field_own_all.py
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
    file_idx = 0
    cell_width = 50
    iteration = 1
    rotation_step = 5
    dummy_container = []

    plt.figure()
    axes = plt.gca()
    tools.draw_field()

    x_range = range(int(-field.x_length * 0.5) + 4*cell_width, int(field.x_length * 0.5), 4*cell_width)
    y_range = range(int(-field.y_length * 0.5) + 4*cell_width, int(field.y_length * 0.5), 4*cell_width)

    # run for the whole field
    for x in x_range:
        for y in y_range:
            time, angle = simulate_best_angle(x, y, state, rotation_step, iteration)
            if not np.isinf(time):
                v = m2d.Vector2(100.0, 0.0).rotate(math.radians(angle))
                axes.arrow(x, y, v.x, v.y, head_width=100, head_length=100, fc='k', ec='k')
            else:
                print("WARNING: Time is Nan")
                v = m2d.Vector2(100.0, 0.0).rotate(math.radians(angle))
                axes.arrow(x, y, v.x, v.y, head_width=100, head_length=100, fc='r', ec='r')
            dummy_container.append([x, y, time, angle])

    plt.show()
    while (os.path.exists('{}{:d}.png'.format('../data/potential_field_generation/potential_field_gen_own', file_idx)) or
           os.path.exists('{}{:d}.pickle'.format('../data/potential_field_generation/potential_field_gen_own', file_idx))):
        file_idx += 1
    plt.savefig('{}{:d}.png'.format('../data/potential_field_generation/potential_field_gen_own', file_idx))
    pickle.dump(dummy_container, open('../data/potential_field_generation/potential_field_gen_own' + str(file_idx) + '.pickle', "wb"))  # make sure not to overwrite anything


if __name__ == "__main__":
    main()
