from matplotlib import pyplot as plt
from matplotlib.patches import Circle
import matplotlib as mpl
import copy
import math
from random import uniform as rand
import LPG
import BISEC as B
import numpy as np
import os.path
import field_info as f

f_inf = (4500, 3000)

def draw_field(ax):
    ax.plot([0 , 0], [-f.y_length * 0.5 , f.y_length * 0.5 ], 'white')  # Middle line

    ax.plot([f.x_opponent_groundline, f.x_opponent_groundline], [f.y_left_sideline, f.y_right_sideline], 'white')  # opponent ground line
    ax.plot([f.x_own_groundline, f.x_own_groundline], [f.y_right_sideline, f.y_left_sideline], 'white')  # own ground line

    ax.plot([f.x_own_groundline, f.x_opponent_groundline], [f.y_left_sideline, f.y_left_sideline], 'white')
    ax.plot([f.x_own_groundline, f.x_opponent_groundline], [f.y_right_sideline, f.y_right_sideline], 'white')

    ax.plot([f.x_opponent_groundline-f.x_penalty_area_length, f.x_opponent_groundline-f.x_penalty_area_length], [-f.y_penalty_area_length*0.5, f.y_penalty_area_length*0.5], 'white')  # opp penalty
    ax.plot([f.x_opponent_groundline, f.x_opponent_groundline-f.x_penalty_area_length], [f.y_penalty_area_length*0.5, f.y_penalty_area_length*0.5], 'white')  # opp penalty
    ax.plot([f.x_opponent_groundline, f.x_opponent_groundline-f.x_penalty_area_length], [-f.y_penalty_area_length*0.5, -f.y_penalty_area_length*0.5], 'white')  # opp penalty

    ax.plot([f.x_own_groundline+f.x_penalty_area_length, f.x_own_groundline+f.x_penalty_area_length], [-f.y_penalty_area_length*0.5, f.y_penalty_area_length*0.5], 'white')  # own penalty
    ax.plot([f.x_own_groundline, f.x_own_groundline+f.x_penalty_area_length], [f.y_penalty_area_length*0.5, f.y_penalty_area_length*0.5], 'white')  # own penalty
    ax.plot([f.x_own_groundline, f.x_own_groundline+f.x_penalty_area_length], [-f.y_penalty_area_length*0.5, -f.y_penalty_area_length*0.5], 'white')  # own penalty

    # Middle Circle
    ax.add_artist(Circle(xy=(0, 0), radius=f.center_circle_radius, fill=False, edgecolor='white'))
    # Penalty Marks
    ax.add_artist(Circle(xy=(f.x_opponent_groundline - f.x_penalty_mark_distance, 0), radius=f.penalty_cross_radius, color='white'))
    ax.add_artist(Circle(xy=(f.x_own_groundline + f.x_penalty_mark_distance, 0), radius=f.penalty_cross_radius, color='white'))

    # Own goal box
    ax.add_artist(Circle(xy=(f.own_goalpost_right.x, f.own_goalpost_right.y), radius=f.goalpost_radius, color='white'))  # GoalPostRight
    ax.add_artist(Circle(xy=(f.own_goalpost_left.x, f.own_goalpost_left.y), radius=f.goalpost_radius, color='white'))  # GoalPostLeft
    ax.plot([f.x_own_groundline, f.x_own_groundline - f.goal_depth], [-f.goal_width*0.5, -f.goal_width*0.5], 'white')  # own goal box
    ax.plot([f.x_own_groundline, f.x_own_groundline - f.goal_depth], [f.goal_width*0.5, f.goal_width*0.5], 'white')  # own goal box
    ax.plot([f.x_own_groundline - f.goal_depth, f.x_own_groundline - f.goal_depth], [-f.goal_width*0.5, f.goal_width*0.5], 'white')  # own goal box

    # Opp GoalBox
    ax.add_artist(Circle(xy=(f.opponent_goalpost_right.x, f.opponent_goalpost_right.y), radius=f.goalpost_radius, color='white'))  # GoalPostRight
    ax.add_artist(Circle(xy=(f.opponent_goalpost_left.x, f.opponent_goalpost_left.y), radius=f.goalpost_radius, color='white'))  # GoalPostLeft
    ax.plot([f.x_opponent_groundline, f.x_opponent_groundline + f.goal_depth], [-f.goal_width*0.5, -f.goal_width*0.5], 'white')  # Opp goal box
    ax.plot([f.x_opponent_groundline, f.x_opponent_groundline + f.goal_depth], [f.goal_width*0.5, f.goal_width*0.5], 'white')  # Opp goal box
    ax.plot([f.x_opponent_groundline + f.goal_depth, f.x_opponent_groundline + f.goal_depth], [-f.goal_width*0.5, f.goal_width*0.5], 'white')  # Opp goal box

    ax.set_axis_bgcolor('#21a540')

def open_file(argv):
    file    = None
    max_exp = None
    if len(argv) > 2:
        file_count = 0
        if len(argv) == 3:
            script, f, filename = argv
        if len(argv) == 4:
            script, f, filename, max_exp = argv
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
    return file, max_exp

def new_experiment(sim_obst):
    obstacles       = []
    LPG_obstacles   = []
    robot_pos       = (0, 0)
    orig_robot_pos  = copy.copy(robot_pos)
    actual_path_LPG = [(0, 0)]
    actual_path_B   = [(0, 0)]
    actual_path_naiv = [(0, 0)]
    all_paths_B     = []
    all_robot_pos   = []
    waypoints       = []
    target          = [math.ceil(rand(-f_inf[0], f_inf[0])), math.ceil(rand(-f_inf[1], f_inf[1]))]
    while len(obstacles) < 9:
        obst_r = 300
        obst_x = np.floor(rand(-f_inf[0], f_inf[0]))
        obst_y = np.floor(rand(-f_inf[1], f_inf[1]))
        if sim_obst == True:
            obst_g = (rand(-60, 60), rand(-60, 60))
        else:
            obst_g = (0, 0)
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
            obstacles.append((obst_x, obst_y, obst_r, obst_g))

    orig_target    = copy.copy(target)
    orig_obstacles = copy.copy(obstacles)
    LPG_obstacles  = copy.copy(obstacles)
    rot            = np.arctan2(target[1], target[0])
    rot_a          = LPG.get_a(target, rot)
    orig_waypoints = LPG.compute_waypoints(target, LPG_obstacles, rot, rot_a)
    return robot_pos, orig_robot_pos, target, orig_target, obstacles, orig_obstacles, LPG_obstacles, waypoints, orig_waypoints, actual_path_B, actual_path_LPG, actual_path_naiv

def draw_path(actual_path_LPG, actual_path_B, actual_path_naiv, ax):
    for k in range(0, len(actual_path_LPG) - 1):
        ax.plot([actual_path_LPG[k][0], actual_path_LPG[k+1][0]], [actual_path_LPG[k][1], actual_path_LPG[k+1][1]], c='red')
    for k in range(0, len(actual_path_B) - 1):
        ax.plot([actual_path_B[k][0], actual_path_B[k+1][0]], [actual_path_B[k][1], actual_path_B[k+1][1]], c='yellow')
    for k in range(0, len(actual_path_naiv) - 1):
        ax.plot([actual_path_naiv[k][0], actual_path_naiv[k+1][0]], [actual_path_naiv[k][1], actual_path_naiv[k+1][1]], c='blue')

def draw_tar_rob(orig_target, robot_pos, ax):
    ax.plot(orig_target[0], orig_target[1], 'x', c='red')
    ax.plot(robot_pos[0], robot_pos[1], 'x', c='red')

def simulate(gait, target, robot_pos, rot, rot_a, actual_path_B, actual_path_LPG, actual_path_naiv, obstacles, orig_obstacles, LPG_obstacles, exp_exec):
    robot_pos = (robot_pos[0] + gait[0], robot_pos[1] + gait[1])
    if exp_exec == 3:
        actual_path_naiv.append((actual_path_naiv[len(actual_path_naiv)-1][0] + gait[0], actual_path_naiv[len(actual_path_naiv)-1][1] + gait[1]))
    if exp_exec == 2:
        actual_path_B.append((actual_path_B[len(actual_path_B)-1][0] + gait[0], actual_path_B[len(actual_path_B)-1][1] + gait[1]))
    if exp_exec == 1:
        actual_path_LPG.append((actual_path_LPG[len(actual_path_LPG)-1][0] + gait[0], actual_path_LPG[len(actual_path_LPG)-1][1] + gait[1]))
        rot    = np.arctan2(target[1], target[0])
        rot_a  = LPG.get_a(target, rot)
        target[0] -= gait[0]
        target[1] -= gait[1]
        for k in range(0, len(LPG_obstacles)):
            LPG_obstacles[k] = (LPG_obstacles[k][0] - gait[0], LPG_obstacles[k][1] - gait[1], LPG_obstacles[k][2], LPG_obstacles[k][3])
            LPG_obstacles[k] = (LPG_obstacles[k][0] + LPG_obstacles[k][3][0], LPG_obstacles[k][1] + LPG_obstacles[k][3][1], LPG_obstacles[k][2], LPG_obstacles[k][3])

    # simulate obstacles
    obstacles, LPG_obstacles = move_obstacles(obstacles, LPG_obstacles, orig_obstacles)
    return gait, target, robot_pos, rot, rot_a, actual_path_B, actual_path_LPG, actual_path_naiv, obstacles, LPG_obstacles

def move_obstacles(obstacles, LPG_obstacles, orig):
    for k in range(0, len(obstacles)):
        obstacles[k] = (obstacles[k][0] + obstacles[k][3][0], obstacles[k][1] + obstacles[k][3][1], obstacles[k][2], obstacles[k][3])

    return obstacles, LPG_obstacles
