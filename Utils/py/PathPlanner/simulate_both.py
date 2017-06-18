from __future__ import division
import LPG
import BISEC as B
from random import uniform as rand
import math
import sys
import numpy as np
from matplotlib import pyplot as plt
from matplotlib.patches import Circle
import matplotlib as mpl
import Queue as Q
import copy
import select
from sys import argv
import os.path
import sim

file = sim.open_file(argv)

keys = {"p": False, "d": False, "e": False, "n": False, "b": False, "r": False, "t": False, "escape": False}
def key_pressed(event):
    keys[str(event.key)] = True
def key_released(event):
    keys[str(event.key)] = False

plt.connect("key_press_event", key_pressed)
#plt.connect("key_release_event", key_released)

# LPG parameters
base           = 1.1789
minimal_cell   = 100
angular_part   = 16
parameter_s = 1

obstacles = [(2561.506295763076, 89.22548767969283, 300), (-3229.8882816473783, 745.2658623693042, 300), (1621.6679149151987, 1067.2159754595705, 300), (4415.865864561594, 2031.630074230553, 300), (794.771189996065, 859.7827650240301, 300), (2055.1103851468215, -2839.405978352935, 300), (4175.285816609758, 533.516366558637, 300), (-1480.9322167897922, -2046.4255889838516, 300), (-2891.519028084903, -1126.5300529647893, 300)]
target    = [2291.0, 979.0]
robot_pos = (0, 0)
orig_robot_pos = copy.copy(robot_pos)

rot   = np.arctan2(target[1], target[0]) # LPG
rot_a = LPG.get_a(target, rot)      # LPG

orig_waypoints = LPG.compute_waypoints(target, obstacles, rot, rot_a)
orig_obstacles = copy.copy(obstacles)
orig_target    = copy.copy(target)

waypoints = []
steps     = []

actual_path_LPG = [(0, 0)]
actual_path_B   = [(0, 0)]
all_paths_B     = []
all_robot_pos   = []

pause = False

exp_count = 0
exp_exec  = 1

history_e = []
go_back_e = False

do_skip_e  = False
do_skip_a  = False
do_restart = False
show_sub   = 0

loop_bool = True

# plot
while loop_bool:
    mpl.rcParams['lines.linewidth'] = 0.5
    plt.clf()
    plt.axis("equal")
    ax = plt.gca()

    # check for key presses
    if keys["p"] == True:
        if pause:
            print("Unpaused.")
        elif not pause:
            print("Paused.")
        pause = not pause
        keys["p"] = False
    if keys["d"] == True:
        print("Experiment " + str(exp_count) + ".")
        print orig_target, orig_obstacles
        keys["d"] = False
    if keys["e"] == True:
        print("Skipping current experiment.")
        do_skip_e = True
        exp_exec = 1
        keys["e"] = False
    if keys["n"] == True:
        print("Switching algorithm.")
        do_skip_a = True
        keys["n"] = False
    if keys["b"] == True:
        if exp_count < len(history_e):
            print("Going back in history.")
            go_back_e = True
        keys["b"] = False
    if keys["r"] == True:
        print("Restarting experiment.")
        do_restart = True
        keys["r"] = False
    if keys["t"] == True:
        if exp_exec == 2:
            show_sub += 1
            if show_sub > 3:
                show_sub = 0
        keys["t"] = False
    if keys["escape"] == True:
        sys.exit()

    # switch algorithm
    if np.absolute(robot_pos[0] - orig_target[0]) < 1 and np.absolute(robot_pos[1] - orig_target[1]) < 1 and exp_exec < 2:
        exp_exec += 1
        obstacles = copy.copy(orig_obstacles)
        target    = copy.copy(orig_target)
        robot_pos = (0, 0)
    if do_skip_a:
        if exp_exec == 2:
            exp_exec = 1
        else:
            exp_exec = 2
        obstacles       = copy.copy(orig_obstacles)
        target          = copy.copy(orig_target)
        robot_pos       = copy.copy(orig_robot_pos)
        actual_path_LPG = [(0, 0)]
        actual_path_B   = [(0, 0)]
    # restart current experiment
    if do_restart:
        target          = copy.copy(orig_target)
        robot_pos       = copy.copy(orig_robot_pos)
        obstacles       = copy.copy(orig_obstacles)
        actual_path_B   = [(0, 0)]
        actual_path_LPG = [(0, 0)]
        do_restart = False
    # save experiment and start next
    if np.absolute(robot_pos[0] - orig_target[0]) < 1 and np.absolute(robot_pos[1] - orig_target[1]) < 1 and exp_exec == 2 or do_skip_e:
        if not do_skip_e:
            if len(argv) == 3:
                loop_bool = eval('exp_count < max_exp')
                if not loop_bool:
                    sys.exit()

            exp_count += 1
            exp_exec   = 1
            print("Experiment " + str(exp_count) + ".")

            if exp_count > 1 and len(argv) > 1:
                file.write(str(actual_path_LPG) + ", " + str(obstacles) + ", " + str(target) + ", " + str(robot_pos))
                file.write("\n")
                file.write(str(actual_path_B) + ", " + str(obstacles) + ", " + str(target) + ", " + str(robot_pos) + ", " + str(robot_radius))
                file.write("\n")

        robot_pos, orig_robot_pos, target, orig_target, obstacles, orig_obstacles, waypoints, orig_waypoints,actual_path_B, actual_path_LPG = sim.new_experiment()

        # stupid experiment???
        do_skip_e = B.stupid_experiment(robot_pos, target, obstacles)
        if not do_skip_e:
            history_e.append((orig_target, orig_obstacles, orig_robot_pos))

    # plot field
    LPG.draw_field(ax)

    # draw obstacles
    if exp_exec == 1:
        LPG.draw_obstacles(ax, robot_pos, orig_obstacles)
    if exp_exec == 2:
        B.draw_obstacles(ax, orig_obstacles)

    # LPG
    if exp_exec == 1:
        waypoints = LPG.compute_waypoints(target, obstacles, rot, rot_a)
        LPG.draw_waypoints(ax, waypoints, robot_pos, rot)

        LPG.draw_LPG(ax, robot_pos, rot)

        gait = LPG.compute_gait(waypoints, target, rot)

    # BISEC
    if exp_exec == 2:
        (gait, path) = B.get_gait(robot_pos, target, obstacles, 0, ax, show_sub)
        ax.plot([robot_pos[0], target[0]],[robot_pos[1], target[1]], c='black')

    # simulate the gait
    if not pause and not do_skip_a:
        gait, target, robot_pos, rot, rot_a, actual_path_B, actual_path_LPG, obstacles = sim.simulate(gait, target, robot_pos, rot, rot_a, actual_path_B, actual_path_LPG, obstacles, exp_exec)

    # draw actual path
    sim.draw_path(actual_path_LPG, actual_path_B, orig_robot_pos, ax)

    # draw target and robot
    sim.draw_tar_rob(orig_target, robot_pos, ax)

    do_skip_a = False

    ax.set_xlim([-5500, 5500])
    ax.set_ylim([-3500, 3500])
    plt.pause(0.000000000001)
