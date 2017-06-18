from matplotlib import pyplot as plt
from matplotlib.patches import Circle
import matplotlib as mpl
import copy
import math
from random import uniform as rand
import LPG
import BISEC as B
import numpy as np

f_inf = (4500, 3000)

def open_file(argv):
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

def new_experiment():
    obstacles       = []
    robot_pos       = (0, 0)
    orig_robot_pos  = copy.copy(robot_pos)
    actual_path_LPG = [(0, 0)]
    actual_path_B   = [(0, 0)]
    all_paths_B     = []
    all_robot_pos   = []
    waypoints       = []
    target          = [math.ceil(rand(-f_inf[0], f_inf[0])), math.ceil(rand(-f_inf[1], f_inf[1]))]
    while len(obstacles) < 9:
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
                if LPG.dist((obst_x, obst_y), k) <= 2*k[2]:
                    do_add = False
                    break
        if do_add:
            obstacles.append((obst_x, obst_y, obst_r))

    orig_target    = copy.copy(target)
    orig_obstacles = copy.copy(obstacles)
    rot            = np.arctan2(target[1], target[0])
    rot_a          = LPG.get_a(target, rot)
    orig_waypoints = LPG.compute_waypoints(target, obstacles, rot, rot_a)
    return robot_pos, orig_robot_pos, target, orig_target, obstacles, orig_obstacles, waypoints, orig_waypoints, actual_path_B, actual_path_LPG

def draw_path(actual_path_LPG, actual_path_B, orig_robot_pos, ax):
    for k in range(0, len(actual_path_LPG) - 1):
        ax.plot([actual_path_LPG[k][0], actual_path_LPG[k+1][0]], [actual_path_LPG[k][1], actual_path_LPG[k+1][1]], c='red')
    for k in range(0, len(actual_path_B) - 1):
        ax.plot([actual_path_B[k][0] + orig_robot_pos[0], actual_path_B[k+1][0] + orig_robot_pos[0]], [actual_path_B[k][1] + orig_robot_pos[1], actual_path_B[k+1][1] + orig_robot_pos[1]], c='yellow')

def draw_tar_rob(orig_target, robot_pos, ax):
    ax.plot(orig_target[0], orig_target[1], 'x', c='red')
    ax.plot(robot_pos[0], robot_pos[1], 'x', c='red')

def simulate(gait, target, robot_pos, rot, rot_a, actual_path_B, actual_path_LPG, obstacles, exp_exec):
    robot_pos = (robot_pos[0] + gait[0], robot_pos[1] + gait[1])
    if exp_exec == 2:
        actual_path_B.append((actual_path_B[len(actual_path_B)-1][0] + gait[0], actual_path_B[len(actual_path_B)-1][1] + gait[1]))
    if exp_exec == 1:
        actual_path_LPG.append((actual_path_LPG[len(actual_path_LPG)-1][0] + gait[0], actual_path_LPG[len(actual_path_LPG)-1][1] + gait[1]))
        rot    = np.arctan2(target[1], target[0])
        rot_a  = LPG.get_a(target, rot)
        target[0] -= gait[0]
        target[1] -= gait[1]
        for k in range(0, len(obstacles)):
            obstacles[k] = (obstacles[k][0] - gait[0], obstacles[k][1] - gait[1], obstacles[k][2])
    return gait, target, robot_pos, rot, rot_a, actual_path_B, actual_path_LPG, obstacles
