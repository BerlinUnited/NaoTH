from matplotlib import pyplot as plt
from matplotlib.patches import Circle
import copy
import math
from random import uniform as rand
import LPG
import BISEC as B
import numpy as np
import os.path
import field_info as f

f_inf = (4500, 3000)


def draw_field_sw(ax):
    ax.plot([0, 0], [-f.y_length * 0.5, f.y_length * 0.5], 'black')  # Middle line

    ax.plot([f.x_opponent_groundline, f.x_opponent_groundline],
            [f.y_left_sideline, f.y_right_sideline], 'black')  # opponent ground line
    ax.plot([f.x_own_groundline, f.x_own_groundline], [f.y_right_sideline, f.y_left_sideline],
            'black')  # own ground line

    ax.plot([f.x_own_groundline, f.x_opponent_groundline], [f.y_left_sideline, f.y_left_sideline],
            'black')
    ax.plot([f.x_own_groundline, f.x_opponent_groundline],
            [f.y_right_sideline, f.y_right_sideline], 'black')

    ax.plot([f.x_opponent_groundline - f.x_penalty_area_length,
             f.x_opponent_groundline - f.x_penalty_area_length],
            [-f.y_penalty_area_length * 0.5, f.y_penalty_area_length * 0.5],
            'black')  # opp penalty
    ax.plot([f.x_opponent_groundline, f.x_opponent_groundline - f.x_penalty_area_length],
            [f.y_penalty_area_length * 0.5, f.y_penalty_area_length * 0.5], 'black')  # opp penalty
    ax.plot([f.x_opponent_groundline, f.x_opponent_groundline - f.x_penalty_area_length],
            [-f.y_penalty_area_length * 0.5, -f.y_penalty_area_length * 0.5],
            'black')  # opp penalty

    ax.plot([f.x_own_groundline + f.x_penalty_area_length,
             f.x_own_groundline + f.x_penalty_area_length],
            [-f.y_penalty_area_length * 0.5, f.y_penalty_area_length * 0.5],
            'black')  # own penalty
    ax.plot([f.x_own_groundline, f.x_own_groundline + f.x_penalty_area_length],
            [f.y_penalty_area_length * 0.5, f.y_penalty_area_length * 0.5], 'black')  # own penalty
    ax.plot([f.x_own_groundline, f.x_own_groundline + f.x_penalty_area_length],
            [-f.y_penalty_area_length * 0.5, -f.y_penalty_area_length * 0.5],
            'black')  # own penalty

    # Middle Circle
    ax.add_artist(Circle(xy=(0, 0), radius=f.center_circle_radius, fill=False, edgecolor='black'))
    # Penalty Marks
    ax.add_artist(Circle(xy=(f.x_opponent_groundline - f.x_penalty_mark_distance, 0),
                         radius=f.penalty_cross_radius, color='black'))
    ax.add_artist(Circle(xy=(f.x_own_groundline + f.x_penalty_mark_distance, 0),
                         radius=f.penalty_cross_radius, color='black'))

    # Own goal box
    ax.add_artist(
        Circle(xy=(f.own_goalpost_right.x, f.own_goalpost_right.y), radius=f.goalpost_radius,
               color='black'))  # GoalPostRight
    ax.add_artist(
        Circle(xy=(f.own_goalpost_left.x, f.own_goalpost_left.y), radius=f.goalpost_radius,
               color='black'))  # GoalPostLeft
    ax.plot([f.x_own_groundline, f.x_own_groundline - f.goal_depth],
            [-f.goal_width * 0.5, -f.goal_width * 0.5], 'black')  # own goal box
    ax.plot([f.x_own_groundline, f.x_own_groundline - f.goal_depth],
            [f.goal_width * 0.5, f.goal_width * 0.5], 'black')  # own goal box
    ax.plot([f.x_own_groundline - f.goal_depth, f.x_own_groundline - f.goal_depth],
            [-f.goal_width * 0.5, f.goal_width * 0.5], 'black')  # own goal box

    # Opp GoalBox
    ax.add_artist(Circle(xy=(f.opponent_goalpost_right.x, f.opponent_goalpost_right.y),
                         radius=f.goalpost_radius, color='black'))  # GoalPostRight
    ax.add_artist(Circle(xy=(f.opponent_goalpost_left.x, f.opponent_goalpost_left.y),
                         radius=f.goalpost_radius, color='black'))  # GoalPostLeft
    ax.plot([f.x_opponent_groundline, f.x_opponent_groundline + f.goal_depth],
            [-f.goal_width * 0.5, -f.goal_width * 0.5], 'black')  # Opp goal box
    ax.plot([f.x_opponent_groundline, f.x_opponent_groundline + f.goal_depth],
            [f.goal_width * 0.5, f.goal_width * 0.5], 'black')  # Opp goal box
    ax.plot([f.x_opponent_groundline + f.goal_depth, f.x_opponent_groundline + f.goal_depth],
            [-f.goal_width * 0.5, f.goal_width * 0.5], 'black')  # Opp goal box

    ax.set_axis_bgcolor('white')


def draw_field(ax):
    ax.plot([0, 0], [-f.y_length * 0.5, f.y_length * 0.5], 'white')  # Middle line

    ax.plot([f.x_opponent_groundline, f.x_opponent_groundline],
            [f.y_left_sideline, f.y_right_sideline], 'white')  # opponent ground line
    ax.plot([f.x_own_groundline, f.x_own_groundline], [f.y_right_sideline, f.y_left_sideline],
            'white')  # own ground line

    ax.plot([f.x_own_groundline, f.x_opponent_groundline], [f.y_left_sideline, f.y_left_sideline],
            'white')
    ax.plot([f.x_own_groundline, f.x_opponent_groundline],
            [f.y_right_sideline, f.y_right_sideline], 'white')

    ax.plot([f.x_opponent_groundline - f.x_penalty_area_length,
             f.x_opponent_groundline - f.x_penalty_area_length],
            [-f.y_penalty_area_length * 0.5, f.y_penalty_area_length * 0.5],
            'white')  # opp penalty
    ax.plot([f.x_opponent_groundline, f.x_opponent_groundline - f.x_penalty_area_length],
            [f.y_penalty_area_length * 0.5, f.y_penalty_area_length * 0.5], 'white')  # opp penalty
    ax.plot([f.x_opponent_groundline, f.x_opponent_groundline - f.x_penalty_area_length],
            [-f.y_penalty_area_length * 0.5, -f.y_penalty_area_length * 0.5],
            'white')  # opp penalty

    ax.plot([f.x_own_groundline + f.x_penalty_area_length,
             f.x_own_groundline + f.x_penalty_area_length],
            [-f.y_penalty_area_length * 0.5, f.y_penalty_area_length * 0.5],
            'white')  # own penalty
    ax.plot([f.x_own_groundline, f.x_own_groundline + f.x_penalty_area_length],
            [f.y_penalty_area_length * 0.5, f.y_penalty_area_length * 0.5], 'white')  # own penalty
    ax.plot([f.x_own_groundline, f.x_own_groundline + f.x_penalty_area_length],
            [-f.y_penalty_area_length * 0.5, -f.y_penalty_area_length * 0.5],
            'white')  # own penalty

    # Middle Circle
    ax.add_artist(Circle(xy=(0, 0), radius=f.center_circle_radius, fill=False, edgecolor='white'))
    # Penalty Marks
    ax.add_artist(Circle(xy=(f.x_opponent_groundline - f.x_penalty_mark_distance, 0),
                         radius=f.penalty_cross_radius, color='white'))
    ax.add_artist(Circle(xy=(f.x_own_groundline + f.x_penalty_mark_distance, 0),
                         radius=f.penalty_cross_radius, color='white'))

    # Own goal box
    ax.add_artist(
        Circle(xy=(f.own_goalpost_right.x, f.own_goalpost_right.y), radius=f.goalpost_radius,
               color='white'))  # GoalPostRight
    ax.add_artist(
        Circle(xy=(f.own_goalpost_left.x, f.own_goalpost_left.y), radius=f.goalpost_radius,
               color='white'))  # GoalPostLeft
    ax.plot([f.x_own_groundline, f.x_own_groundline - f.goal_depth],
            [-f.goal_width * 0.5, -f.goal_width * 0.5], 'white')  # own goal box
    ax.plot([f.x_own_groundline, f.x_own_groundline - f.goal_depth],
            [f.goal_width * 0.5, f.goal_width * 0.5], 'white')  # own goal box
    ax.plot([f.x_own_groundline - f.goal_depth, f.x_own_groundline - f.goal_depth],
            [-f.goal_width * 0.5, f.goal_width * 0.5], 'white')  # own goal box

    # Opp GoalBox
    ax.add_artist(Circle(xy=(f.opponent_goalpost_right.x, f.opponent_goalpost_right.y),
                         radius=f.goalpost_radius, color='white'))  # GoalPostRight
    ax.add_artist(Circle(xy=(f.opponent_goalpost_left.x, f.opponent_goalpost_left.y),
                         radius=f.goalpost_radius, color='white'))  # GoalPostLeft
    ax.plot([f.x_opponent_groundline, f.x_opponent_groundline + f.goal_depth],
            [-f.goal_width * 0.5, -f.goal_width * 0.5], 'white')  # Opp goal box
    ax.plot([f.x_opponent_groundline, f.x_opponent_groundline + f.goal_depth],
            [f.goal_width * 0.5, f.goal_width * 0.5], 'white')  # Opp goal box
    ax.plot([f.x_opponent_groundline + f.goal_depth, f.x_opponent_groundline + f.goal_depth],
            [-f.goal_width * 0.5, f.goal_width * 0.5], 'white')  # Opp goal box

    ax.set_axis_bgcolor('#21C540')


def open_file(argv):
    the_file = None
    max_exp = None
    filename = None
    draw = 't'
    if len(argv) > 2:
        draw = argv[2]

    if len(argv) > 3:
        file_count = 0
        if len(argv) == 4:
            script, f, draw, filename = argv
        if len(argv) == 5:
            script, f, draw, filename, max_exp = argv
            max_exp = int(max_exp)

        orig_filename = copy.copy(filename)
        if os.path.isfile(filename + '.npy'):
            file_count = 1
            filename = filename + str(file_count)
            while os.path.isfile(filename + '.npy'):
                file_count += 1
                filename = orig_filename + str(file_count)
        if file_count > 0:
            print("The specified file already exists, renaming to " + filename + ".")
        # file = open(filename, 'w')
    if draw is not 't':
        draw = False
    else:
        draw = True
    return the_file, max_exp, filename, draw


def new_experiment(sim_obst):
    obstacles = []
    LPG_obstacles = []
    robot_pos = (0, 0)
    orig_robot_pos = copy.copy(robot_pos)
    actual_path_LPG = [(0, 0)]
    actual_path_B = [(0, 0)]
    actual_path_naiv = [(0, 0)]
    all_paths_B = []
    all_robot_pos = []
    waypoints = []
    target = [math.ceil(rand(-f_inf[0], f_inf[0])), math.ceil(rand(-f_inf[1], f_inf[1]))]
    while len(obstacles) < 9:
        obst_r = 300
        obst_x = np.floor(rand(-f_inf[0], f_inf[0]))
        obst_y = np.floor(rand(-f_inf[1], f_inf[1]))
        if sim_obst:
            obst_g = (rand(-4500, 4500), rand(-3000, 3000))
            while np.absolute(target[0] - obst_g[0]) <= 300 or np.absolute(
                    target[1] - obst_g[1]) <= 300:
                obst_g = (rand(-4500, 4500), rand(-3000, 3000))
        else:
            obst_g = (0, 0)

        do_add = True
        if obst_x <= (obst_r + B.robot_radius) and obst_x >= -1 * (
                obst_r + B.robot_radius) and obst_y <= (
                obst_r + B.robot_radius) and obst_y >= -1 * (obst_r + B.robot_radius):
            do_add = False
        if B.dist((obst_x, obst_y), target) <= obst_r + B.robot_radius:
            do_add = False
        if len(obstacles) > 0:
            for k in obstacles:
                if LPG.dist((obst_x, obst_y), k) <= 2 * k[2]:
                    do_add = False
                    break
        if do_add:
            obstacles.append((obst_x, obst_y, obst_r, obst_g))

    orig_target = copy.copy(target)
    orig_obstacles = copy.copy(obstacles)
    LPG_obstacles = copy.copy(obstacles)
    rot = np.arctan2(target[1], target[0])
    rot_a = LPG.get_a(target, rot)
    orig_waypoints = LPG.compute_waypoints(target, LPG_obstacles, rot, rot_a)
    return robot_pos, orig_robot_pos, target, orig_target, obstacles, orig_obstacles, LPG_obstacles, waypoints, orig_waypoints, actual_path_B, actual_path_LPG, actual_path_naiv


def draw_path(actual_path_LPG, actual_path_B, actual_path_naiv, ax, algorithm):
    if algorithm == 1:
        for k in range(0, len(actual_path_LPG) - 1):
            ax.plot([actual_path_LPG[k][0], actual_path_LPG[k + 1][0]],
                    [actual_path_LPG[k][1], actual_path_LPG[k + 1][1]], c='red')
    if algorithm == 2:
        for k in range(0, len(actual_path_B) - 1):
            ax.plot([actual_path_B[k][0], actual_path_B[k + 1][0]],
                    [actual_path_B[k][1], actual_path_B[k + 1][1]], c='red')
    if algorithm == 3:
        for k in range(0, len(actual_path_naiv) - 1):
            ax.plot([actual_path_naiv[k][0], actual_path_naiv[k + 1][0]],
                    [actual_path_naiv[k][1], actual_path_naiv[k + 1][1]], c='red')


def draw_all_paths(LPG, BI, robot_LPG, robot_B, ax, algorithm):
    if algorithm == 1:
        for k in range(0, len(LPG)):
            if k % 20 == 0:
                for l in range(0, len(LPG[k]) - 1):
                    ax.plot([LPG[k][l][0] + robot_LPG[k][0], LPG[k][l + 1][0] + robot_LPG[k][0]],
                            [LPG[k][l][1] + robot_LPG[k][1], LPG[k][l + 1][1] + robot_LPG[k][1]],
                            c='black')
    if algorithm == 2:
        for k in range(0, len(BI)):
            if k % 10 == 0:
                for l in range(0, len(BI[k]) - 1):
                    ax.plot([BI[k][l][0], BI[k][l + 1][0]], [BI[k][l][1], BI[k][l + 1][1]],
                            c='black')


def draw_tar_rob(orig_target, robot_pos, ax):
    ax.plot(orig_target[0], orig_target[1], 'x', c='red')
    ax.plot(robot_pos[0], robot_pos[1], 'x', c='yellow')


def simulate(gait, target, robot_pos, rot, rot_a, actual_path_B, actual_path_LPG, actual_path_naiv,
             obstacles, orig_obstacles, LPG_obstacles, algorithm, sim_obst):
    robot_pos = (robot_pos[0] + gait[0], robot_pos[1] + gait[1])
    if algorithm == 3:
        actual_path_naiv.append((actual_path_naiv[len(actual_path_naiv) - 1][0] + gait[0],
                                 actual_path_naiv[len(actual_path_naiv) - 1][1] + gait[1]))
    if algorithm == 2:
        actual_path_B.append((actual_path_B[len(actual_path_B) - 1][0] + gait[0],
                              actual_path_B[len(actual_path_B) - 1][1] + gait[1]))
    if algorithm == 1:
        actual_path_LPG.append((actual_path_LPG[len(actual_path_LPG) - 1][0] + gait[0],
                                actual_path_LPG[len(actual_path_LPG) - 1][1] + gait[1]))
        rot = np.arctan2(target[1], target[0])
        rot_a = LPG.get_a(target, rot)
        target[0] -= gait[0]
        target[1] -= gait[1]
        for k in range(0, len(LPG_obstacles)):
            LPG_obstacles[k] = (
                LPG_obstacles[k][0] - gait[0], LPG_obstacles[k][1] - gait[1], LPG_obstacles[k][2],
                LPG_obstacles[k][3])

    if sim_obst:
        for k in range(0, len(obstacles)):
            direction = (
                obstacles[k][3][0] - obstacles[k][0], obstacles[k][3][1] - obstacles[k][1])
            distance = np.linalg.norm(direction)
            gait_obst = direction / distance * min(60, max(-60, distance))

            if np.isnan(gait_obst[0]) or np.isnan(gait_obst[1]):
                gait_obst = (0, 0)

            obstacles[k] = (
                obstacles[k][0] + gait_obst[0], obstacles[k][1] + gait_obst[1], obstacles[k][2],
                obstacles[k][3])

            LPG_obstacles[k] = (
                LPG_obstacles[k][0] + gait_obst[0], LPG_obstacles[k][1] + gait_obst[1],
                LPG_obstacles[k][2], LPG_obstacles[k][3])

    return gait, target, robot_pos, rot, rot_a, actual_path_B, actual_path_LPG, actual_path_naiv, obstacles, LPG_obstacles
