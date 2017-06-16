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

if len(argv) > 1:
    file_count = 0
    if len(argv) == 2:
        script, filename = argv
    if len(argv) == 3:
        script, filename, max_exp = argv
        max_exp = int(max_exp)

    orig_filename    = copy.copy(filename)
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
    i,o,e = select.select([sys.stdin],[],[],0.0001)
    for s in i:
        if s == sys.stdin:
            input = sys.stdin.readline()
            return True
    return False

f_inf = (4500, 3000)

# LPG parameters
base           = 1.1789
minimal_cell   = 100
angular_part   = 16
parameter_s = 1

obstacles = [(2000, 0, 300)]#[(1600, 0, 300), (1600, 1000, 300), (1600, 500, 300), (1600, 1500, 300), (1600, -1000, 300), (1600, -500, 300)]
target    = [4000, 0]
robot_pos = (0, 0)
orig_robot_pos = copy.copy(robot_pos)

rot   = np.arctan2(target[1], target[0]) # LPG
rot_a = LPG.cell_angle(target, rot)      # LPG

orig_waypoints = LPG.compute_waypoints_LPG(target, obstacles, rot, rot_a)
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
exp_exec  = 2
loop_bool = True

tmp_paths_count = 0

# plot
while loop_bool:
    if heard_pause():
        pause = not pause
    while pause:
        if heard_pause():
            pause = not pause

    # switch algorithm
    """if np.absolute(robot_pos[0] - orig_target[0]) < 1 and np.absolute(robot_pos[1] - orig_target[1]) < 1 and exp_exec < 2:
        exp_exec += 1
        obstacles = copy.copy(orig_obstacles)
        target    = copy.copy(orig_target)
        robot_pos = (0, 0)
    # save experiment and start next
    if np.absolute(robot_pos[0] - orig_target[0]) < 1 and np.absolute(robot_pos[1] - orig_target[1]) < 1 and exp_exec == 2:
        if len(argv) == 3:
            loop_bool = eval('exp_count < max_exp')
            if not loop_bool:
                sys.exit()
        exp_count += 1
        #exp_exec   = 1
        print("Experiment " + str(exp_count) + ".")

        if exp_count > 1 and len(argv) > 1:
            file.write(str(actual_path_LPG) + ", " + str(obstacles) + ", " + str(target) + ", " + str(robot_pos))
            file.write("\n")
            file.write(str(actual_path_B) + ", " + str(obstacles) + ", " + str(target) + ", " + str(robot_pos) + ", " + str(robot_radius))
            file.write("\n")

        obstacles       = []
        robot_pos       = (0, 0)
        orig_robot_pos  = copy.copy(robot_pos)
        actual_path_LPG = [(0, 0)]
        actual_path_B   = [(0, 0)]
        all_paths_B     = []
        all_robot_pos   = []
        waypoints       = []
        target          = [math.ceil(rand(-f_inf[0], f_inf[0])), math.ceil(rand(-f_inf[1], f_inf[1]))]
        while len(obstacles) < 16:
            obst_r = 300
            obst_x = rand(-f_inf[0], f_inf[0])
            obst_y = rand(-f_inf[1], f_inf[1])
            do_add = True
            if obst_x <= (obst_r + B.robot_radius) and obst_x >= -1*(obst_r + B.robot_radius) and obst_y <= (obst_r + B.robot_radius) and obst_y >= -1*(obst_r + B.robot_radius):
                do_add = False
            if B.dist((obst_x, obst_y), target) <= obst_r + B.robot_radius:
                do_add = False
            if len(obstacles) > 0:
                for k in obstacles:
                    if LPG.dist_between((obst_x, obst_y), k) <= 2*k[2]:
                        do_add = False
                        break
            if do_add:
                obstacles.append((obst_x, obst_y, obst_r))

        orig_target    = copy.copy(target)
        orig_obstacles = copy.copy(obstacles)
        rot            = np.arctan2(target[1], target[0])
        rot_a          = LPG.cell_angle(target, rot)
        orig_waypoints = LPG.compute_waypoints_LPG(target, obstacles, rot, rot_a)"""

    mpl.rcParams['lines.linewidth'] = 0.5
    plt.clf()
    plt.axis("equal")
    ax = plt.gca()

    # plot field
    LPG.draw_field(ax)

    # draw obstacles
    #LPG.draw_obstacles(ax, robot_pos, orig_obstacles)
    B.draw_obstacles(ax, orig_obstacles)

    # LPG
    if exp_exec == 1:
        waypoints = LPG.compute_waypoints_LPG(target, obstacles, rot, rot_a)
        LPG.draw_waypoints(ax, waypoints, robot_pos, rot)

        LPG.draw_LPG(ax, robot_pos, rot)

        # compute next part of actual path
        gait = LPG.compute_gait(waypoints, target, rot)
        #actual_path_LPG.append((actual_path_LPG[len(actual_path_LPG)-1][0] + gait[0], actual_path_LPG[len(actual_path_LPG)-1][1] + gait[1]))

    # BISEC
    if exp_exec == 2:
        (gait, path) = B.get_gait(robot_pos, target, obstacles, 0, ax)
        actual_path_B.append((actual_path_B[len(actual_path_B)-1][0] + gait[0], actual_path_B[len(actual_path_B)-1][1] + gait[1]))
        #all_paths_B.append([robot_pos] + path)
        #all_robot_pos.append(robot_pos)

    # simulate the gait
    robot_pos = (robot_pos[0] + gait[0], robot_pos[1] + gait[1])
    if exp_exec == 1:
        rot    = np.arctan2(target[1], target[0])
        rot_a  = LPG.cell_angle(target, rot)
        target[0] -= math.ceil(gait[0])
        target[1] -= math.ceil(gait[1])
        for k in range(0, len(obstacles)):
            obstacles[k] = (obstacles[k][0] - gait[0], obstacles[k][1] - gait[1], obstacles[k][2])

    # draw tmp paths for BISEC
    """if exp_exec == 2:
        for l in range(0, len(all_paths_B)):
            for k in range(0, len(all_paths_B[l]) - 1):
                ax.plot([all_paths_B[l][k][0] + all_robot_pos[l][0], all_paths_B[l][k+1][0] + all_robot_pos[l][0]], [all_paths_B[l][k][1] + all_robot_pos[l][1], all_paths_B[l][k+1][1] + all_robot_pos[l][1]], c='black')
"""
    # draw actual path
    for k in range(0, len(actual_path_LPG) - 1):
        ax.plot([actual_path_LPG[k][0], actual_path_LPG[k+1][0]], [actual_path_LPG[k][1], actual_path_LPG[k+1][1]], c='red')
    for k in range(0, len(actual_path_B) - 1):
        ax.plot([actual_path_B[k][0] + orig_robot_pos[0], actual_path_B[k+1][0] + orig_robot_pos[0]], [actual_path_B[k][1] + orig_robot_pos[1], actual_path_B[k+1][1] + orig_robot_pos[1]], c='yellow')

    ax.plot(robot_pos[0], robot_pos[1], 'x', c='yellow')
    #for k in path:
        #ax.plot(k[0] + robot_pos[0], k[1] + robot_pos[1], 'x', c='red')
        #ax.plot(k[0], k[1], 'x', c='red')

    # draw target
    ax.plot(orig_target[0], orig_target[1], 'x', c='red')

    ax.set_xlim([-5500, 5500])
    ax.set_ylim([-3500, 3500])
    plt.pause(0.000000000001)
