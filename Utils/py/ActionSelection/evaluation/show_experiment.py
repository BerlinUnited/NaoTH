from __future__ import division
import os, sys
import inspect

cmd_subfolder = os.path.realpath(os.path.abspath(os.path.join(os.path.split(inspect.getfile(inspect.currentframe()))[0], "..")))
if cmd_subfolder not in sys.path:
    sys.path.insert(0, cmd_subfolder)


import math

import pickle
import numpy as np
from matplotlib import pyplot as plt
from matplotlib.patches import Circle
from tools import tools
from tools import field_info as field




def plot_start_positions(exp):
    # test for iterating over all frames
    plt.clf()
    axes = plt.gca()
    tools.draw_field(axes)

    for i in range(len(exp['frames'])):
        start_x = exp['frames'][i]['sim']['optimal_one'][0].state.pose.translation.x
        start_y = exp['frames'][i]['sim']['optimal_one'][0].state.pose.translation.y
        axes.add_artist(Circle(xy=(start_x, start_y), radius=100, fill=False, edgecolor='white'))

    plt.show()


def extractValues(exp, strategy, getValue):
  return np.array([getValue(e) for frame in exp['frames'] for e in frame['sim'][strategy]])
    
def plot_histogram(exp):

    num = exp['frames'][0]['sim']
    f, ax = plt.subplots(1, 3, sharey=True)

    for i, strategy in enumerate(exp['frames'][0]['sim']):
      print strategy
      values = extractValues(exp, strategy, lambda x: x.turn_around_ball)
      ax[i].hist(np.abs(np.degrees(values)), range=[0, 180], rwidth=1, bins=18)
      ax[i].set_title(strategy)

    #plt.legend()
    plt.show()


def plot_matrix(exp):
    all_rotations = []
    all_ball_turns = []
    all_walking = []

    num_frames = len(exp['frames'])

    for frame in range(num_frames):
        e = exp['frames'][frame]['sim']['optimal_one']
        trace_length = len(e)
        start_x = e[0].state.pose.translation.x
        start_y = e[0].state.pose.translation.y
        rotation = 0
        turn_around_ball = 0
        walk_distance = 0

        for i in range(trace_length):
            rotation += np.abs(e[i].rotate)
            turn_around_ball += np.abs(e[i].turn_around_ball)
            walk_distance += np.abs(e[i].walk_dist)

        all_rotations.append([start_x, start_y, rotation])
        all_ball_turns.append([start_x, start_y, turn_around_ball])
        all_walking.append([start_x, start_y, walk_distance])

    plt.clf()
    axes = plt.gca()
    tools.draw_field(axes)

    data = all_rotations

    nx = {}
    ny = {}
    for pos in data:
        x, y, value = pos
        nx[x] = x
        ny[y] = y

    nxi = np.array(sorted(nx.keys()))
    nyi = np.array(sorted(ny.keys()))

    for i, v in enumerate(nxi):
        nx[v] = i

    for i, v in enumerate(nyi):
        ny[v] = i

    f = np.zeros((len(ny), len(nx)))

    for pos in data:
        x, y, value = pos

        value = value

        if np.isnan(value):
            f[ny[y], nx[x]] = 0
        else:
            f[ny[y], nx[x]] = value

    x_step = y_step = 300

    x_range = range(int(-field.x_length / 2), int(field.x_length / 2) + x_step, x_step)
    y_range = range(int(-field.y_length / 2), int(field.y_length / 2) + y_step, y_step)

    axes.pcolor(x_range, y_range, f, cmap="jet", alpha=0.8)
    plt.show()


if __name__ == "__main__":
    
    data_prefix = os.path.realpath(os.path.abspath(os.path.join(cmd_subfolder,"../data")))
    data_prefix = "./data/"
    file = data_prefix + "simulation_2.pickle"
    print "read file: "+ file
    
    experiment = pickle.load(open(file, "rb"))

    # plot_start_positions(experiment)

    # plot_matrix(experiment)

    
    plot_histogram(experiment)
