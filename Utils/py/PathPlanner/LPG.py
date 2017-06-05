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

base           = 1.1789
minimal_cell   = 10
angular_part   = 16

parameter_s = 1

def distance(coords):
    x = coords[0]
    y = coords[1]
    return math.floor(math.log(((math.sqrt(x*x + y*y) * (base - 1)) / minimal_cell) + 1, base))
def inv_distance(r):
    return (np.exp(np.log(base)*r) - 1) * minimal_cell / (base - 1)

def cell_angle(coords, rot):
    x = coords[0]
    y = coords[1]
    return math.floor((angular_part / (2*math.pi)) * (angle(x, y) - rot) + 0.5)
def angle(x,y):
    if (x is 0):
        return np.arctan2(y, 1)
    else:
        return np.arctan2(y, x)

def cell_mid(coords, rot): # returns cell mid from cartesian coordinates
    x = coords[0]
    y = coords[1]
    x = np.absolute(x)
    y = np.absolute(y)
    if np.sign(x) < 0:
        x_sign = -1
    else:
        x_sign = 1
    if np.sign(y) < 0:
        y_sign = -1
    else:
        y_sign = 1

    # distance and angle for the middle of the cell
    dist = distance((x, y))
    angl = cell_angle((x, y), rot) * (2*math.pi/16)

    x_new = np.cos(angl) * (((np.power(base, dist + 0.5) - 1) * minimal_cell) / (base-1))
    y_new = np.sin(angl) * (((np.power(base, dist + 0.5) - 1) * minimal_cell) / (base-1))
    return (x_new, y_new)

def cell_mid_from_polar(r, a, rot):   # returns cell mid from polar coordinates
    prd = (((np.power(base, r+0.5) - 1) * minimal_cell) / (base - 1))
    return (np.cos(a * (2*math.pi/16) + rot) * prd, np.sin(a * (2*math.pi/16) + rot) * prd)

def get_cell(coords, rot):
    return (distance(coords), cell_angle(coords, rot))

def dist_between(a, b):
    (x1, y1) = (a[0], a[1])
    (x2, y2) = (b[0], b[1])
    return np.sqrt(np.power(x1 - x2, 2) + np.power(y1 - y2, 2))
def dist_between_cell(a, b, rot):
    (x1, y1) = cell_mid_from_polar(a[0], a[1], rot)
    (x2, y2) = cell_mid_from_polar(b[0], b[1], rot)
    return np.sqrt(np.power(x1 - x2, 2) + np.power(y1 - y2, 2))

def obst_func(cell, obst, rot):   # obst is obstacle coordinates in x, y
    r_f              = obst[2]
    cell_mid         = cell_mid_from_polar(cell[0], cell[1], rot)
    dist_to_obst_mid = dist_between(cell_mid, obst)
    obst_dist_to_mid = dist_between(obst, (0, 0))
    r_d              = obst_dist_to_mid / 10

    # parameters
    a = r_f - r_d  # cost of constant part
    r = r_f + r_d  # radius of constant part
    s = parameter_s*r        # radius of linear decreasing part

    return np.maximum(np.minimum(1 - ((dist_to_obst_mid - r) / s), 1), 0) * a

# A STAR IMPLEMENTATION
def a_star_search(start, goal, obstacles, rot):
    openlist = Q.PriorityQueue()
    closedlist = set()
    openlist.put((0, start))
    came_from = {}
    cost_so_far = {}
    came_from[start] = None
    cost_so_far[start] = 0

    while not openlist.empty():
        current = openlist.get()[1]

        if current == goal:
            break

        closedlist.add(current)

        for r in [0, -1, 1]:
            for a in [0, -1, 1]:
                the_next = (current[0] + r, current[1] + a)
                if the_next in closedlist:
                    continue

                # initialize cost_so_far
                if math.isnan(cost_so_far[current]):
                    cost_so_far[current] = 0

                # cell cost without obstacles
                new_cost = cost_so_far[current] + dist_between_cell(current, the_next, rot)

                # add obstacle costs to cell
                for obst in obstacles:
                    new_cost += obst_func(the_next, obst, rot)

                # add to or update openlist
                if the_next not in cost_so_far or new_cost < cost_so_far[the_next]:
                    cost_so_far[the_next] = new_cost
                    priority = new_cost + dist_between_cell(the_next, goal, rot)
                    openlist.put((priority, the_next))
                    came_from[the_next] = current

    return came_from, cost_so_far

def compute_waypoints_LPG(tar, obstacles, rot, rot_a):
    tar_cell = get_cell(tar, rot)
    start = (tar_cell[0], tar_cell[1])  # target only occupies only cell
    target = (0, rot_a)

    # compute obstacle radius

    (a, b) = a_star_search(start, target, obstacles, rot)
    the_next = target
    the_path = [the_next]
    while a[the_next] in a:
        the_next = a[the_next]
        the_path.append(the_next)

    return the_path

def compute_gait(the_path, target, rot):
    (x, y) = (0, 0)
    for k in range(0, len(the_path)):
        (x, y) = cell_mid_from_polar(the_path[k][0], the_path[k][1], rot)
        if (np.absolute(x) >= 40) or (np.absolute(y) >= 40):
            break

    x = min(max(x, -40), 40)
    y = min(max(y, -40), 40)

    if np.sqrt(np.power(x, 2) + np.power(y, 2)) > np.sqrt(np.power(target[0], 2) + np.power(target[1], 2)):
        (x, y) = target

    return (x, y)

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

def draw_LPG(ax, x_off, y_off, rot):
    a_length = 2*math.pi / angular_part
    radius = 60000
    a = (np.arange(0, angular_part) + 0.5) * a_length
    x = np.cos(a + rot) * radius
    y = np.sin(a + rot) * radius

    # draw rings
    for k in range(0, 30):
        rad = inv_distance(k)
        ax.add_artist(Circle(xy=(0+x_off, 0+y_off), radius=rad, fill=False, color='black', alpha=.25))

    # draw angular partitions
    for k in range(0, len(x)):
        ax.plot([0+x_off, x[k]+x_off], [0+y_off, y[k]+y_off], 'black', alpha=.25)

def draw_waypoints(ax, waypoints, x_off, y_off, rot):
    # draw waypoint cells
    for k in waypoints:
        (way_x, way_y) = cell_mid_from_polar(k[0], k[1], rot)
        ax.plot(way_x+x_off, way_y+y_off, ".", c='blue')

def draw_obstacles(ax, x_off, y_off, obstacles):
    # draw obstacles
    if obstacles:
        obstacle_set = set()
        for k in obstacles:
            ax.add_artist(Circle(xy=(k[0], k[1]), radius=k[2]+(dist_between(k, (x_off, y_off))/10) + (k[2]+(dist_between(k, (x_off, y_off))/10) * parameter_s), fill=True, color='blue', alpha=.25))
            ax.add_artist(Circle(xy=(k[0], k[1]), radius=k[2]+dist_between(k, (x_off, y_off))/100, fill=True, color='red', alpha=.25))
            ax.add_artist(Circle(xy=(k[0], k[1]), radius=10, fill=True, color='black'))
