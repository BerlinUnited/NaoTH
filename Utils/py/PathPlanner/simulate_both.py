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
import naiv as N
import time

the_file, max_exp, filename, draw = sim.open_file(argv)

keys = {"p": False, "d": False, "e": False, "n": False, "r": False, "t": False, "escape": False,
        "f": False}


def key_pressed(event):
    keys[str(event.key)] = True


def key_released(event):
    keys[str(event.key)] = False


plt.connect("key_press_event", key_pressed)
# plt.connect("key_release_event", key_released)

# LPG parameters
base = 1.1789
minimal_cell = 100
angular_part = 16
parameter_s = 1

LPG_obstacles = []
obstacles = []
target = [0, 0]
robot_pos = (0, 0)
orig_robot_pos = copy.copy(robot_pos)

rot = np.arctan2(target[1], target[0])  # LPG
rot_a = LPG.get_a(target, rot)  # LPG

orig_waypoints = LPG.compute_waypoints(target, LPG_obstacles, rot, rot_a)
orig_obstacles = copy.copy(obstacles)
orig_target = copy.copy(target)

waypoints = []
steps = []

actual_path_LPG = [(0, 0)]
actual_path_B = [(0, 0)]
actual_path_naiv = [(0, 0)]

all_robot = []
all_target = []
all_obstacle = []
all_path_LPG = []
all_path_B = []
all_path_naiv = []

times_LPG = []
times_B = []
times_naiv = []

pause = False

exp_count = 0
algorithm = 1

go_back_e = False
do_skip_e = False
do_skip_a = False
do_restart = False
show_sub = False
deadlock = False
loop_bool = True

save_current = False
current_all_path_B = []
current_all_path_LPG = []
current_all_path_naiv = []
current_target = []
current_obstacles = []

sim_obst = True
if len(argv) > 1:
    if argv[1] is not 'f':
        sim_obst = True
    else:
        sim_obst = False

# plot
while loop_bool:
    if draw:
        # mpl.rcParams['lines.linewidth'] = 0.5
        plt.clf()
        plt.axis("equal")
        ax = plt.gca()
    else:
        ax = None

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
        print(orig_target, orig_obstacles)
        keys["d"] = False
    if len(argv) <= 3:
        if keys["e"] == True:
            print("Skipping current experiment.")
            do_skip_e = True
            # algorithm = 1
            keys["e"] = False
        if keys["n"] == True:
            print("Switching algorithm.")
            do_skip_a = True
            keys["n"] = False
        if keys["r"] == True:
            print("Restarting experiment.")
            do_restart = True
            keys["r"] = False
        if keys["f"] == True:
            save_current = True
            print("Saving current.")
            keys["f"] = False
    if keys["t"] == True:
        show_sub = not show_sub
        keys["t"] = False
    if keys["escape"] == True:
        sys.exit()

    # switch algorithm
    if np.absolute(robot_pos[0] - orig_target[0]) < 1 and np.absolute(
            robot_pos[1] - orig_target[1]) < 1 and algorithm < 3 or (algorithm < 3 and deadlock):
        algorithm += 1
        obstacles = copy.copy(orig_obstacles)
        LPG_obstacles = copy.copy(orig_obstacles)
        target = copy.copy(orig_target)
        robot_pos = (0, 0)
        deadlock = False
    # skip current algorithm
    if do_skip_a:
        algorithm += 1
        if algorithm == 4:
            algorithm = 1
        LPG_obstacles = copy.copy(orig_obstacles)
        obstacles = copy.copy(orig_obstacles)
        target = copy.copy(orig_target)
        robot_pos = copy.copy(orig_robot_pos)
        actual_path_LPG = [(0, 0)]
        actual_path_B = [(0, 0)]
        actual_path_naiv = [(0, 0)]
    # restart current experiment
    if do_restart:
        target = copy.copy(orig_target)
        robot_pos = copy.copy(orig_robot_pos)
        obstacles = copy.copy(orig_obstacles)
        LPG_obstacles = copy.copy(orig_obstacles)
        actual_path_B = [(0, 0)]
        actual_path_LPG = [(0, 0)]
        actual_path_naiv = [(0, 0)]
        do_restart = False
    # save experiment and start next
    if np.absolute(robot_pos[0] - orig_target[0]) < 1 and np.absolute(
            robot_pos[1] - orig_target[1]) < 1 and algorithm == 3 or do_skip_e or (
            deadlock and algorithm == 3):
        deadlock = False
        if not do_skip_e:
            exp_count += 1

            algorithm = 1

            if exp_count > 1 and len(argv) > 3:
                all_robot.append(copy.copy(orig_robot_pos))
                all_target.append(copy.copy(orig_target))
                all_obstacle.append(copy.copy(orig_obstacles))
                all_path_LPG.append(copy.copy(actual_path_LPG))
                all_path_B.append(copy.copy(actual_path_B))
                all_path_naiv.append(copy.copy(actual_path_naiv))
                everything = (
                all_robot, all_target, all_obstacle, all_path_LPG, all_path_B, all_path_naiv,
                times_LPG, times_B, times_naiv)
                np.save(filename, everything)

            if save_current:
                current_all_path_LPG.append(copy.copy(actual_path_LPG))
                current_all_path_B.append(copy.copy(actual_path_B))
                current_all_path_naiv.append(copy.copy(actual_path_naiv))
                current_target.append(orig_target)
                current_obstacles.append(orig_obstacles)
                current_everything = (
                current_target, current_all_path_LPG, current_all_path_B, current_all_path_naiv,
                current_obstacles)
                np.save("current", current_everything)
                save_current = False
                print("Current saved.")

            if len(argv) == 5:
                loop_bool = eval('exp_count <= max_exp')
                if not loop_bool:
                    sys.exit()

            print("Experiment " + str(exp_count) + ".")

        robot_pos, orig_robot_pos, target, orig_target, obstacles, orig_obstacles, LPG_obstacles, waypoints, orig_waypoints, actual_path_B, actual_path_LPG, actual_path_naiv = sim.new_experiment(
            sim_obst)

        # stupid experiment???
        do_skip_e = B.stupid_experiment(robot_pos, target, obstacles)
        # if not do_skip_e:
        #    print(orig_target, orig_obstacles)

    # plot field
    if draw:
        sim.draw_field(ax)

    # draw obstacles
    if draw:
        B.draw_obstacles(ax, obstacles)
        # LPG.draw_obstacles(ax, robot_pos, LPG_obstacles)

    # LPG
    if algorithm == 1:
        start = time.time()
        waypoints = LPG.compute_waypoints(target, LPG_obstacles, rot, rot_a)
        if waypoints is not None:
            gait = LPG.compute_gait(waypoints, target, rot)
            times_LPG.append(time.time() - start)
            # if draw:
            # LPG.draw_waypoints(ax, waypoints, robot_pos, rot)

            # if draw:
            # LPG.draw_LPG(ax, robot_pos, rot)

    # BISEC
    if algorithm == 2:
        start = time.time()
        (gait, waypoints_B) = B.get_gait(robot_pos, target, obstacles, 0, ax, show_sub)
        times_B.append(time.time() - start)
        # if draw:
        # ax.plot([robot_pos[0], target[0]],[robot_pos[1], target[1]], c='black') # straight trajectory line from robot_pos to target

    # Naiv
    if algorithm == 3:
        start = time.time()
        gait = N.compute_gait(robot_pos, target, obstacles)
        times_naiv.append(time.time() - start)

    # simulate the gait
    if not pause and not do_skip_a:
        gait, target, robot_pos, rot, rot_a, actual_path_B, actual_path_LPG, actual_path_naiv, obstacles, LPG_obstacles = sim.simulate(
            gait, target, robot_pos, rot, rot_a, actual_path_B, actual_path_LPG, actual_path_naiv,
            obstacles, orig_obstacles, LPG_obstacles, algorithm, sim_obst)
        deadlock = (len(actual_path_naiv) > 200 and algorithm == 3) or (
                    len(actual_path_LPG) > 200 and algorithm == 1) or (
                               len(actual_path_B) > 200 and algorithm == 2)
        if deadlock:
            if algorithm == 1:
                actual_path_LPG = []
                print(" ------------ DEADLOCK LPG ------------")
            elif algorithm == 2:
                print(" ------------ DEADLOCK BISEC ------------")
                actual_path_B = []
            elif algorithm == 3:
                print(" ------------ DEADLOCK NAIV ------------")
                actual_path_naiv = []

    # draw actual path
    if draw:
        sim.draw_path(actual_path_LPG, actual_path_B, actual_path_naiv, ax, algorithm)

    # draw target and robot
    if draw:
        sim.draw_tar_rob(orig_target, robot_pos, ax)

    do_skip_a = False

    if draw:
        ax.set_xlim([-5500, 5500])
        ax.set_ylim([-3500, 3500])
        plt.pause(0.000000000001)
