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

def draw_obstacles(ax, obstacles):
    # draw obstacles
    if obstacles:
        for k in obstacles:
            ax.add_artist(Circle(xy=(k[0], k[1]), radius=k[2], fill=True, color='red', alpha=.25))
            ax.add_artist(Circle(xy=(k[0], k[1]), radius=10, fill=True, color='black'))

def draw_robot(ax, robot_pos):
    ax.add_artist(Circle(xy=(robot_pos[0], robot_pos[1]), radius=robot_radius, fill=True, color='black', alpha=.5))

def draw_target(ax, target):
    ax.plot(target[0], target[1], 'x', color='red')

def draw_steps(ax, robot_pos, steps, col):
    x_dist = robot_pos[0]
    y_dist = robot_pos[1]
    path   = []
    for k in steps:
        path.append((k[0] + x_dist, k[1] + y_dist))
        x_dist += k[0]
        y_dist += k[1]
    for k in range(0, len(path) - 1):
        ax.plot([path[k][0], path[k+1][0]], [path[k][1], path[k+1][1]], c=col)

def dist(start, target):
    return np.sqrt(np.power(start[0] - target[0], 2) + np.power(start[1] - target[1], 2))

def hit_obstacle(start, steps, obstacles):
    (x_dist, y_dist) = (0, 0)

    for k in steps:
        for l in obstacles:
            if dist(l, (k[0] + x_dist + start[0], k[1] + y_dist + start[1])) <= l[2] + robot_radius:
                return (True, l)#(x_dist, y_dist))
        x_dist += k[0]
        y_dist += k[1]

    return (False, (0, 0))

def has_collided(vec, obstacles):
    for k in obstacles:
        if dist(vec, k) <= k[2] + robot_radius:
            return True
    return False

def compute_steps(start, target):
    steps            = []
    (x_dist, y_dist) = start
    target           = (target[0] - start[0], target[1] - start[1])

    while True:
        target_dist    = dist((0, 0), target)
        gait_unit_vec  = (target[0] / target_dist, target[1] / target_dist)
        max_steplength = np.absolute(min(60, max(-60, min(target_dist / gait_unit_vec[0], target_dist / gait_unit_vec[1]))))
        gait           = (gait_unit_vec[0] * max_steplength, gait_unit_vec[1] * max_steplength)

        if math.isnan(gait[0]) or math.isnan(gait[1]):
            break
        if gait[0] is 0 and gait[1] is 0:
            break
        if gait[0] is -0 and gait[1] is -0:
            break

        # check if target is closer than gait
        if np.sqrt(np.power(gait[0], 2) + np.power(gait[1], 2)) > np.sqrt(np.power(target[0], 2) + np.power(target[1], 2)):
            gait = target
            steps.append(gait)
            break

        steps.append(gait)

        target  = (target[0] - gait[0], target[1] - gait[1])
        x_dist += gait[0]
        y_dist += gait[1]

    return steps

def compute_sub_target(start, target, collision, obstacles, ax, sign):
    robot_diameter_count = 1

    target_vec  = (target[0] - start[0], target[1] - start[1])
    target_dist = dist(start, target_vec)
    unit_vec    = (target_vec[0] / target_dist, target_vec[1] / target_dist)

    orth_vec1 = (unit_vec[1] * -1, unit_vec[0])
    orth_vec2 = (unit_vec[1], unit_vec[0] * -1)

    if sign == 1:
        sub_target = (orth_vec1[0] * (robot_radius * robot_diameter_count) + collision[0], orth_vec1[1] * (robot_radius * robot_diameter_count) + collision[1])
    else:
        sub_target = (orth_vec2[0] * (robot_radius * robot_diameter_count) + collision[0], orth_vec2[1] * (robot_radius * robot_diameter_count) + collision[1])

    alt_sub_targets = []

    while has_collided(sub_target, obstacles):
        alt_sub_targets.append(sub_target)
        robot_diameter_count += 1
        if sign == 1:
            sub_target = (orth_vec1[0] * (robot_radius * robot_diameter_count) + collision[0], orth_vec1[1] * (robot_radius * robot_diameter_count) + collision[1])
        else:
            sub_target = (orth_vec2[0] * (robot_radius * robot_diameter_count) + collision[0], orth_vec2[1] * (robot_radius * robot_diameter_count) + collision[1])

    ax.plot(sub_target[0], sub_target[1], 'x', color='white')
    #for k in alt_sub_targets:
    #    ax.plot(k[0], k[1], 'x', color='white')

    return sub_target

def compute_path(start, target, obstacles, depth, ax, sign):
    collision = start
    steps     = compute_steps(start, target)

    if depth > 6:
        return steps

    (has_collided, col_pos) = hit_obstacle(start, steps, obstacles)

    if has_collided:
        sub_target = compute_sub_target(start, target, col_pos, obstacles, ax, sign)
        steps      = compute_path(start, sub_target, obstacles, depth+1, ax, sign) + compute_path(sub_target, target, obstacles, depth+1, ax, sign)

    return steps
