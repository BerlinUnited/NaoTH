from __future__ import division
import sys
import math
import numpy as np
from matplotlib import pyplot as plt
from matplotlib.patches import Circle
import matplotlib as mpl
import Queue as Q
import copy
import field_info as f

robot_radius = 300

def draw_field(ax, x_off, y_off):
    ax.plot([0 +x_off, 0+x_off], [-f.y_length * 0.5 + y_off, f.y_length * 0.5 + y_off], 'white')  # Middle line

    ax.plot([f.x_opponent_groundline+x_off, f.x_opponent_groundline+x_off], [f.y_left_sideline+y_off, f.y_right_sideline+y_off], 'white')  # opponent ground line
    ax.plot([f.x_own_groundline+x_off, f.x_own_groundline+x_off], [f.y_right_sideline+y_off, f.y_left_sideline+y_off], 'white')  # own ground line

    ax.plot([f.x_own_groundline+x_off, f.x_opponent_groundline+x_off], [f.y_left_sideline+y_off, f.y_left_sideline+y_off], 'white')
    ax.plot([f.x_own_groundline+x_off, f.x_opponent_groundline+x_off], [f.y_right_sideline+y_off, f.y_right_sideline+y_off], 'white')

    ax.plot([f.x_opponent_groundline-f.x_penalty_area_length+x_off, f.x_opponent_groundline-f.x_penalty_area_length+x_off], [-f.y_penalty_area_length*0.5+y_off, f.y_penalty_area_length*0.5+y_off], 'white')  # opp penalty
    ax.plot([f.x_opponent_groundline+x_off, f.x_opponent_groundline-f.x_penalty_area_length+x_off], [f.y_penalty_area_length*0.5+y_off, f.y_penalty_area_length*0.5+y_off], 'white')  # opp penalty
    ax.plot([f.x_opponent_groundline+x_off, f.x_opponent_groundline-f.x_penalty_area_length+x_off], [-f.y_penalty_area_length*0.5+y_off, -f.y_penalty_area_length*0.5+y_off], 'white')  # opp penalty

    ax.plot([f.x_own_groundline+f.x_penalty_area_length+x_off, f.x_own_groundline+f.x_penalty_area_length+x_off], [-f.y_penalty_area_length*0.5+y_off, f.y_penalty_area_length*0.5+y_off], 'white')  # own penalty
    ax.plot([f.x_own_groundline+x_off, f.x_own_groundline+f.x_penalty_area_length+x_off], [f.y_penalty_area_length*0.5+y_off, f.y_penalty_area_length*0.5+y_off], 'white')  # own penalty
    ax.plot([f.x_own_groundline+x_off, f.x_own_groundline+f.x_penalty_area_length+x_off], [-f.y_penalty_area_length*0.5+y_off, -f.y_penalty_area_length*0.5+y_off], 'white')  # own penalty

    # Middle Circle
    ax.add_artist(Circle(xy=(0+x_off, 0+y_off), radius=f.center_circle_radius, fill=False, edgecolor='white'))
    # Penalty Marks
    ax.add_artist(Circle(xy=(f.x_opponent_groundline - f.x_penalty_mark_distance+x_off, 0+y_off), radius=f.penalty_cross_radius, color='white'))
    ax.add_artist(Circle(xy=(f.x_own_groundline + f.x_penalty_mark_distance+x_off, 0+y_off), radius=f.penalty_cross_radius, color='white'))

    # Own goal box
    ax.add_artist(Circle(xy=(f.own_goalpost_right.x+x_off, f.own_goalpost_right.y+y_off), radius=f.goalpost_radius, color='white'))  # GoalPostRight
    ax.add_artist(Circle(xy=(f.own_goalpost_left.x+x_off, f.own_goalpost_left.y+y_off), radius=f.goalpost_radius, color='white'))  # GoalPostLeft
    ax.plot([f.x_own_groundline+x_off, f.x_own_groundline - f.goal_depth+x_off], [-f.goal_width*0.5+y_off, -f.goal_width*0.5+y_off], 'white')  # own goal box
    ax.plot([f.x_own_groundline+x_off, f.x_own_groundline - f.goal_depth+x_off], [f.goal_width*0.5+y_off, f.goal_width*0.5+y_off], 'white')  # own goal box
    ax.plot([f.x_own_groundline - f.goal_depth+x_off, f.x_own_groundline - f.goal_depth+x_off], [-f.goal_width*0.5+y_off, f.goal_width*0.5+y_off], 'white')  # own goal box

    # Opp GoalBox
    ax.add_artist(Circle(xy=(f.opponent_goalpost_right.x+x_off, f.opponent_goalpost_right.y+y_off), radius=f.goalpost_radius, color='white'))  # GoalPostRight
    ax.add_artist(Circle(xy=(f.opponent_goalpost_left.x+x_off, f.opponent_goalpost_left.y+y_off), radius=f.goalpost_radius, color='white'))  # GoalPostLeft
    ax.plot([f.x_opponent_groundline+x_off, f.x_opponent_groundline + f.goal_depth+x_off], [-f.goal_width*0.5+y_off, -f.goal_width*0.5+y_off], 'white')  # Opp goal box
    ax.plot([f.x_opponent_groundline+x_off, f.x_opponent_groundline + f.goal_depth+x_off], [f.goal_width*0.5+y_off, f.goal_width*0.5+y_off], 'white')  # Opp goal box
    ax.plot([f.x_opponent_groundline + f.goal_depth+x_off, f.x_opponent_groundline + f.goal_depth+x_off], [-f.goal_width*0.5+y_off, f.goal_width*0.5+y_off], 'white')  # Opp goal box

    ax.set_axis_bgcolor('green')

def draw_obstacles(ax, x_off, y_off, obstacles):
    # draw obstacles
    if obstacles:
        for k in obstacles:
            ax.add_artist(Circle(xy=(k[0], k[1]), radius=k[2], fill=True, color='red', alpha=.25))
            ax.add_artist(Circle(xy=(k[0], k[1]), radius=10, fill=True, color='black'))

def draw_target(ax, target):
    for k in target:
        ax.plot(k[0], k[1], 'x', color='red')

def draw_steps(ax, x_off, y_off, steps):
    x_dist = 0
    y_dist = 0
    for k in steps:
        ax.plot(k[0] + x_dist + x_off, k[1] + y_dist + y_off, 'x', color='black')
        x_dist += k[0]
        y_dist += k[1]

def draw_walked_path(ax, path):
    for k in range(0, len(path) - 1):
        ax.plot([path[k][0], path[k+1][0]], [path[k][1], path[k+1][1]], c='red')

def dist(start, target):
    return np.sqrt(np.power(start[0] - target[0], 2) + np.power(start[1] - target[1], 2))

def hit_obstacle(obstacles, gait_pos):
    for k in range(0, len(obstacles)):
        if dist((obstacles[k][0], obstacles[k][1]), gait_pos) <= obstacles[k][2] + robot_radius:
            return k
    return -1

def compute_steps(tar, obstacles, x_off, y_off):
    target       = copy.copy(tar)
    steps        = []
    x_dist       = 0
    y_dist       = 0
    obstacle_hit = -1

    while len(steps) < 20:
        tar_dist       = dist((0, 0), target)
        gait_unit_vec  = (target[0] / tar_dist, target[1] / tar_dist)
        max_steplength = np.absolute(min(60, max(-60, min(tar_dist / gait_unit_vec[0], tar_dist / gait_unit_vec[1]))))
        gait           = (gait_unit_vec[0] * max_steplength, gait_unit_vec[1] * max_steplength)

        if math.isnan(gait[0]) or math.isnan(gait[1]):
            break
        if gait[0] is 0 and gait[1] is 0:
            break
        if gait[0] is -0 and gait[1] is -0:
            break

        # check for obstacles
        obstacle_hit = hit_obstacle(obstacles, (x_off + x_dist + gait[0], y_off + y_dist + gait[1]))

        if obstacle_hit > -1:
            break

        # check if target is closer than gait
        if np.sqrt(np.power(gait[0], 2) + np.power(gait[1], 2)) > np.sqrt(np.power(target[0], 2) + np.power(target[1], 2)):
            gait = target

        steps.append(gait)

        target = (target[0] - gait[0], target[1] - gait[1])
        x_dist += gait[0]
        y_dist += gait[1]

    return steps, obstacle_hit

def compute_sub_target(obstacle, target, x_off, y_off):
    tmp_target = (target[0] - x_off, target[1] - y_off)
    unit_vec  = (tmp_target[0] / dist((0, 0), tmp_target), tmp_target[1] / dist((0, 0), tmp_target))
    sub_target1 = (unit_vec[1] * -1 * (robot_radius * 1.5 + obstacle[2]) + obstacle[0], unit_vec[0] * (robot_radius * 1.5 + obstacle[2]) + obstacle[1])
    sub_target2 = (unit_vec[1] * (robot_radius * 1.5 + obstacle[2]) + obstacle[0], unit_vec[0] * -1 * (robot_radius * 1.5 + obstacle[2]) + obstacle[1])

    dist1       = dist(target, sub_target1)
    dist2       = dist(target, sub_target2)

    if dist1 > dist2:
        sub_target = sub_target2
    else:
        sub_target = sub_target1

    return sub_target
