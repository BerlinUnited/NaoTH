import os
import math
import pickle
import numpy as np
from matplotlib import pyplot as plt

from naoth.math import *
from potentialfield_generation.field_own import main as simulate_best_angle
from tools import field_info as field
from tools import tools
from state import State

"""
This file simulates the  best angle for all positions on the field by simulation all the steps necessary to 
score a goal and compares the time for each rotation. The rotation with the shortest time to goal is the best.
    
Example:
    run without any parameters

        $ python generate_potential_field.py
"""


def main():
    state = State()
    file_idx = 0

    x_step = 200
    y_step = 200

    iteration = 1
    rotation_step = 5
    dummy_container = []

    show_image = False

    axes = plt.gca()
    tools.draw_field(axes)

    # use this to iterate over the whole green
    field_x_range = range(int(-field.x_field_length * 0.5),
                          int(field.x_field_length * 0.5) + x_step, x_step)
    field_y_range = range(int(-field.y_field_length * 0.5),
                          int(field.y_field_length * 0.5) + y_step, y_step)

    # use this to just iterate over the playing field
    x_range = range(int(-field.x_length * 0.5), int(field.x_length * 0.5) + x_step, x_step)
    y_range = range(int(-field.y_length * 0.5), int(field.y_length * 0.5) + x_step, y_step)

    # run for the whole field
    for x in x_range:
        for y in y_range:
            time, angle = simulate_best_angle(x, y, state, rotation_step, iteration)
            if not np.isinf(time):
                v = Vector2(100.0, 0.0).rotate(math.radians(angle))
                axes.arrow(x, y, v.x, v.y, head_width=100, head_length=100, fc='k', ec='k')
            else:
                print("WARNING: Time is Nan")
                v = Vector2(100.0, 0.0).rotate(math.radians(angle))
                axes.arrow(x, y, v.x, v.y, head_width=100, head_length=100, fc='r', ec='r')
            dummy_container.append([x, y, time, angle])

    # make sure not to overwrite anything
    while (os.path.exists(
            '{}{:d}.png'.format('../data/potential_field_generation/potential_field_gen_own',
                                file_idx)) or
           os.path.exists(
               '{}{:d}.pickle'.format('../data/potential_field_generation/potential_field_gen_own',
                                      file_idx))):
        file_idx += 1
    plt.savefig('{}{:d}.png'.format('../data/potential_field_generation/potential_field_gen_own',
                                    file_idx))
    pickle.dump(dummy_container, open(
        '../data/potential_field_generation/potential_field_gen_own' + str(file_idx) + '.pickle',
        "wb"))

    if show_image:
        plt.show()


if __name__ == "__main__":
    main()
