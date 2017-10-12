from __future__ import division
import os
import sys
import math
import inspect
import pickle
import numpy as np
from matplotlib import pyplot as plt
from matplotlib.patches import Circle
from tools import tools
from tools import field_info as field
from state import State

cmd_subfolder = os.path.realpath(os.path.abspath(os.path.join(os.path.split(inspect.getfile( inspect.currentframe() ))[0],"..")))
if cmd_subfolder not in sys.path:
    sys.path.insert(0, cmd_subfolder)


if __name__ == "__main__":

    plt.clf()
    axes = plt.gca()
    tools.draw_field(axes)
    data_prefix = "D:/RoboCup/Paper-Repos/2017-humanoids-action-selection/data/"

    experiment = pickle.load(open(str(data_prefix) + "simulation_0.pickle", "rb"))

    # print(experiment['frames'][0]['sim']['optimal_one'][1].turn_around_ball)

    # test for iterating over all frames
    """
    num_frames = len(experiment['frames'])
    for i in range(num_frames):
        test_x = experiment['frames'][i]['sim']['optimal_one'][0].state.pose.translation.x
        test_y = experiment['frames'][i]['sim']['optimal_one'][0].state.pose.translation.y
        axes.add_artist(Circle(xy=(test_x, test_y), radius=100, fill=False, edgecolor='white'))

    plt.show()
    """
    # test for iterating over one trace
    """
    fixed_frame = 435
    trace_length = len(experiment['frames'][fixed_frame]['sim']['optimal_one'])
    for i in range(trace_length):
        test_x = experiment['frames'][fixed_frame]['sim']['optimal_one'][i].state.pose.translation.x
        test_y = experiment['frames'][fixed_frame]['sim']['optimal_one'][i].state.pose.translation.y
        axes.add_artist(Circle(xy=(test_x, test_y), radius=100, fill=False, edgecolor='white'))

        print("Step: " + str(i))
        print("Strategy: " + str(experiment['frames'][fixed_frame]['sim']['optimal_one'][i].strategy))
        print("ActionList: " + str(experiment['frames'][fixed_frame]['sim']['optimal_one'][i].action_list))
        print("Goal: " + str(experiment['frames'][fixed_frame]['sim']['optimal_one'][i].goal_scored))
        print("Inside: " + str(experiment['frames'][fixed_frame]['sim']['optimal_one'][i].inside_field))
        print("ChosenAction: " + str(experiment['frames'][fixed_frame]['sim']['optimal_one'][i].selected_action_idx))
        print("TurnBall: " + str(experiment['frames'][fixed_frame]['sim']['optimal_one'][i].turn_around_ball))
        print("Rotate: " + str(experiment['frames'][fixed_frame]['sim']['optimal_one'][i].rotate))
        print("WalkDist: " + str(experiment['frames'][fixed_frame]['sim']['optimal_one'][i].walk_dist))
        print("------------------------------------------------")
    plt.show()
    """
    # get turn times for each position

    num_frames = len(experiment['frames'])
    all_ball_turns = []

    for frame in range(num_frames):
        exp = experiment['frames'][frame]['sim']['optimal_one']
        trace_length = len(exp)
        ball_turns = 0
        for i in range(trace_length):
            #ball_turns += np.abs(experiment['frames'][frame]['sim']['optimal_one'][i].turn_around_ball)
            #ball_turns = max(ball_turns, np.abs(exp[i].turn_around_ball))
            ball_turns += np.abs(exp[i].walk_dist)

        test_x = exp[0].state.pose.translation.x
        test_y = exp[0].state.pose.translation.y
        all_ball_turns.append([test_x, test_y, ball_turns])

    nx = {}
    ny = {}
    for pos in all_ball_turns:
        x, y, c_num_turn_ball_degrees = pos
        nx[x] = x
        ny[y] = y

    nxi = np.array(sorted(nx.keys()))
    nyi = np.array(sorted(ny.keys()))

    for i, v in enumerate(nxi):
        nx[v] = i

    for i, v in enumerate(nyi):
        ny[v] = i

    f = np.zeros((len(ny), len(nx)))

    for pos in all_ball_turns:
        x, y, c_num_turn_ball_degrees = pos

        value = c_num_turn_ball_degrees

        if np.isnan(value):  # or np.isnan(time_particle):
            f[ny[y], nx[x]] = 0
        else:
            f[ny[y], nx[x]] = value  # / np.max(np.abs([time_particle,time_old]))

    x_step = 300
    y_step = 300
    x_range = range(int(-field.x_length / 2), int(field.x_length / 2) + x_step, x_step)
    y_range = range(int(-field.y_length / 2), int(field.y_length / 2) + y_step, y_step)

    axes.pcolor(x_range, y_range, f, cmap="jet", alpha=0.8)  # , vmin=-1, vmax = 1
    plt.show()
