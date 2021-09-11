from __future__ import division
import LPG
from random import uniform as rand
import math
import sys
import numpy as np
from matplotlib import pyplot as plt
import matplotlib as mpl
import copy
import select
from sys import argv
import os.path

if len(argv) > 1:
    file_count = 0
    if len(argv) == 2:
        script, filename = argv
    if len(argv) == 3:
        script, filename, max_exp = argv
        max_exp = int(max_exp)

    orig_filename = copy.copy(filename)
    if os.path.isfile(filename):
        file_count = 1
        filename = filename + str(file_count)
        while os.path.isfile(filename):
            file_count += 1
            filename = orig_filename + str(file_count)
    if file_count > 0:
        print("The specified file already exists, renaming to " + filename + ".")
    file = open(filename, 'w')


def heard_pause():
    i, o, e = select.select([sys.stdin], [], [], 0.0001)
    for s in i:
        if s == sys.stdin:
            input = sys.stdin.readline()
            return True
    return False


f_inf = (4500, 3000)

# LPG parameters
base = 1.1789
minimal_cell = 100
angular_part = 16
parameter_s = 1

obstacles = []
target = [0, 0]

robot_pos = (0, 0)
rot = np.arctan2(target[1], target[0])
rot_a = LPG.get_a(target, rot)

orig_waypoints = LPG.compute_waypoints(target, obstacles, rot, rot_a)
orig_obstacles = copy.copy(obstacles)
orig_target = copy.copy(target)

actual_path = [(0, 0)]

pause = False

exp_count = 0
loop_bool = True

# plot
while loop_bool:
    if heard_pause():
        pause = not pause
    while pause:
        if heard_pause():
            pause = not pause

    mpl.rcParams['lines.linewidth'] = 0.5
    plt.clf()
    plt.axis("equal")
    ax = plt.gca()

    # plot field
    LPG.draw_field(ax)

    LPG.draw_obstacles(ax, robot_pos, orig_obstacles)

    waypoints = LPG.compute_waypoints(target, obstacles, rot, rot_a)
    LPG.draw_waypoints(ax, waypoints, robot_pos, rot)

    LPG.draw_LPG(ax, robot_pos, rot)

    # compute next part of actual path
    gait = LPG.compute_gait(waypoints, target, rot)
    actual_path.append((actual_path[len(actual_path) - 1][0] + gait[0],
                        actual_path[len(actual_path) - 1][1] + gait[1]))

    # simulate the gait
    robot_pos = (robot_pos[0] + math.ceil(gait[0]), robot_pos[1] + math.ceil(gait[1]))
    rot = np.arctan2(target[1], target[0])
    rot_a = LPG.get_a(target, rot)
    target[0] -= math.ceil(gait[0])
    target[1] -= math.ceil(gait[1])
    for k in range(0, len(obstacles)):
        obstacles[k] = (obstacles[k][0] - gait[0], obstacles[k][1] - gait[1], obstacles[k][2])

    # draw actual path
    for k in range(0, len(actual_path) - 1):
        ax.plot([actual_path[k][0], actual_path[k + 1][0]],
                [actual_path[k][1], actual_path[k + 1][1]], c='red')

    # draw target
    ax.plot(orig_target[0], orig_target[1], 'x', c='red')

    # save experiment and start next
    if robot_pos[0] == orig_target[0] and robot_pos[1] == orig_target[1]:
        if len(argv) == 3:
            loop_bool = eval('exp_count < max_exp')
            if not loop_bool:
                sys.exit()
        exp_count += 1
        print("Experiment " + str(exp_count) + ".")

        if exp_count > 1 and len(argv) > 1:
            file.write(str(actual_path) + ", " + str(obstacles) + ", " + str(target) + ", " + str(
                robot_pos))
            file.write("\n")

        obstacles = []
        robot_pos = (0, 0)
        actual_path = [(0, 0)]
        waypoints = []
        target = [math.ceil(rand(-f_inf[0], f_inf[0])), math.ceil(rand(-f_inf[1], f_inf[1]))]
        while len(obstacles) < 9:
            obst_r = 300
            obst_x = rand(-f_inf[0], f_inf[0])
            obst_y = rand(-f_inf[1], f_inf[1])
            if obst_x <= (obst_r + LPG.robot_radius) and obst_x >= -1 * (
                    obst_r + LPG.robot_radius) and obst_y <= (
                    obst_r + LPG.robot_radius) and obst_y >= -1 * (obst_r + LPG.robot_radius):
                continue
            if LPG.dist((obst_x, obst_y), target) <= obst_r + LPG.robot_radius:
                continue
            do_add = True
            if len(obstacles) > 0:
                for k in obstacles:
                    if LPG.dist((obst_x, obst_y), k) <= 2 * k[2]:
                        do_add = False
                        break
            if do_add:
                obstacles.append((obst_x, obst_y, obst_r))
        for k in obstacles:
            while LPG.dist(k, target) <= 2 * k[2]:
                target = [math.ceil(rand(-f_inf[0], f_inf[0])),
                          math.ceil(rand(-f_inf[1], f_inf[1]))]

        orig_target = copy.copy(target)
        orig_obstacles = copy.copy(obstacles)
        rot = np.arctan2(target[1], target[0])
        rot_a = LPG.get_a(target, rot)
        orig_waypoints = LPG.compute_waypoints(target, obstacles, rot, rot_a)

    ax.set_xlim([-5500, 5500])
    ax.set_ylim([-3500, 3500])
    plt.pause(0.000000000001)
