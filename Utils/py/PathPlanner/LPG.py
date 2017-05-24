from __future__ import division
import sys
import math
import numpy as np
from matplotlib import pyplot as plt
from matplotlib.patches import Circle
import matplotlib as mpl
import Queue as Q

base           = 1.1789
minimal_cell   = 100
angular_part   = 16

parameter_s = 1

def distance(coords):
    x = coords[0]
    y = coords[1]
    return math.floor(math.log(((math.sqrt(x*x + y*y) * (base - 1)) / minimal_cell) + 1, base))
def inv_distance(r):
    return (np.exp(np.log(base)*r) - 1) * minimal_cell / (base - 1)

def cell_angle(coords):
    x = coords[0]
    y = coords[1]
    return math.floor((angular_part / (2*math.pi)) * angle(x, y) + 0.5)
def angle(x,y):
    if (x is 0):
        return np.arctan2(y, 1)
    else:
        return np.arctan2(y, x)

def cell_mid(coords): # returns cell mid from cartesian coordinates
    x = coords[0]
    y = coords[1]
    x = np.absolute(x)
    y = np.absolute(y)

    # distance and angle for the middle of the cell
    dist = distance((x, y))
    angl = cell_angle((x, y)) * (2*math.pi/16)

    x_new = np.cos(angl) * (((np.power(base, dist + 0.5) - 1) * minimal_cell) / (base-1))
    y_new = np.sin(angl) * (((np.power(base, dist + 0.5) - 1) * minimal_cell) / (base-1))
    if np.sign(x) < 0:
        x_new *= -1
    if np.sign(y) < 0:
        y_new *= -1
    return (x_new, y_new)

def cell_mid_from_polar(r, a):   # returns cell mid from polar coordinates
    prd = (((np.power(base, r+0.5) - 1) * minimal_cell) / (base - 1))
    return (np.cos(a * (2*math.pi/16)) * prd, np.sin(a * (2*math.pi/16)) * prd)

def get_cell(coords):
    return (distance(coords), cell_angle(coords))

def dist_between(a, b):
    (x1, y1) = (a[0], a[1])
    (x2, y2) = (b[0], b[1])
    return np.sqrt(np.power(x1 - x2, 2) + np.power(y1 - y2, 2))
def dist_between_cell(a, b):
    (x1, y1) = cell_mid_from_polar(a[0], a[1])
    (x2, y2) = cell_mid_from_polar(b[0], b[1])
    return np.sqrt(np.power(x1 - x2, 2) + np.power(y1 - y2, 2))

def obst_func(cell, obst):   # obst is obstacle coordinates in x, y
    r_f              = obst[2]
    cell_mid         = cell_mid_from_polar(cell[0], cell[1])
    dist_to_obst_mid = dist_between(cell_mid, obst)
    obst_dist_to_mid = dist_between(obst, (0, 0))
    r_d              = obst_dist_to_mid / 100

    # parameters
    a = r_f - r_d  # cost of constant part
    r = r_f + r_d  # radius of constant part
    s = parameter_s*r        # radius of linear decreasing part

    return np.maximum(np.minimum(1 - ((dist_to_obst_mid - r) / s), 1), 0) * a

# A STAR IMPLEMENTATION
def a_star_search(start, goal, obstacles):
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

        for r in [0, -1]:
            for a in [0, 1, -1]:
                the_next = (current[0] + r, current[1] + a)
                if the_next in closedlist:
                    continue
                # initialize cost_so_far
                if math.isnan(cost_so_far[current]):
                    cost_so_far[current] = 0

                # cell cost without obstacles
                new_cost = cost_so_far[current] + dist_between_cell(current, the_next)

                # add obstacle costs to cell
                #old_cost = new_cost
                for obst in obstacles:
                    new_cost += obst_func(the_next, obst)
                #print(the_next)
                #print(new_cost - old_cost)
                # add to or update openlist
                if the_next not in cost_so_far or new_cost < cost_so_far[the_next]:
                    cost_so_far[the_next] = new_cost
                    priority = new_cost + dist_between_cell(the_next, goal)
                    openlist.put((priority, the_next))
                    came_from[the_next] = current

    return came_from, cost_so_far

def compute_waypoints_LPG(tar, obstacles):
    start = (get_cell(tar)[0], get_cell(tar)[1])  # target only occupies only cell
    target = (0, 0)

    # compute obstacle radius

    (a, b) = a_star_search(start, target, obstacles)
    the_next = (0, 0)
    the_path = [the_next]
    while a[the_next] in a:
        the_next = a[the_next]
        the_path.append(the_next)

    # compute gait vector for next step
    x = 0
    y = 0
    k = 0
    """while np.absolute(x) < 40 or np.absolute(y) < 40:
        the_mid = cell_mid(the_path[k])
        x += the_mid[0]
        y += the_mid[1]
        k += 1"""
    return the_path

def compute_waypoints_bisection(target):
    return [[0, 0]]

def draw(obstacles, target, algorithm):
    if not target:
        print("Need a target!")
        sys.exit()
    elif len(target) > 2:
        print("Too many targets!")
        sys.exit()

    # plotting field
    a_length = 2*math.pi / angular_part
    radius = 6000
    a = (np.arange(0, angular_part) + 0.5) * a_length
    x = np.cos(a) * radius
    y = np.sin(a) * radius

    mpl.rcParams['lines.linewidth'] = 0.5
    plt.clf()
    plt.axis("equal")
    ax = plt.gca()

    # draw obstacles
    if (obstacles is not []):
        obstacle_set = set()
        for k in obstacles:
            obstacle_set.add(cell_mid(k))
            ax.add_artist(Circle(xy=(k[0], k[1]), radius=k[2]+(dist_between(k, (0, 0))/100) + (k[2]+(dist_between(k, (0, 0))/100) * parameter_s), fill=True, color='green', alpha=.25))
            ax.add_artist(Circle(xy=(k[0], k[1]), radius=k[2]+dist_between(k, (0, 0))/100, fill=True, color='red', alpha=.25))
            ax.add_artist(Circle(xy=(k[0], k[1]), radius=10, fill=True, color='black'))

    # draw rings
    for k in range(1, 17):
        rad = inv_distance(k)
        ax.add_artist(Circle(xy=(0, 0), radius=rad, fill=False, color='black'))

    # draw angular partitions
    for k in range(0, len(x)):
        ax.plot([0, x[k]], [0, y[k]], 'black')

    # draw target
    ax.plot(cell_mid(target)[0], cell_mid(target)[1], 'x', c='green')

    # draw path
    if algorithm is "LPG":
        waypoints = compute_waypoints_LPG(target, obstacles)
    if algorithm is "own":
        waypoints = compute_waypoints_bisection(target)

    # mark waypoint cells
    for k in waypoints:
        (x, y) = cell_mid_from_polar(k[0], k[1])
        ax.plot(x, y, ">", c='blue')

    ax.set_xlim([-4500, 4500])
    ax.set_ylim([-3000, 3000])

    plt.pause(1)
