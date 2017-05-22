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

def distance(x, y):
    return math.floor(math.log(((math.sqrt(x*x + y*y) * (base - 1)) / minimal_cell) + 1, base))
def inv_distance(r):
    return (np.exp(np.log(base)*r) - 1) * minimal_cell / (base - 1)

def cell_angle(x, y):
    return math.floor((angular_part / (2*math.pi)) * angle(x, y) + 0.5)
def angle(x,y):
    if (x is 0):
        return np.arctan2(y, 1)
    else:
        return np.arctan2(y, x)

def cell_mid(x, y):
    if (x is not 0):
        vorz_x = np.absolute(x) / x
    else:
        vorz_x = 1
    if (y is not 0):
        vorz_y = np.absolute(y) / y
    else:
        vorz_y = 1
    x = np.absolute(x)
    y = np.absolute(y)

    # distance and angle for the middle of the cell
    dist = distance(x, y)
    angl = cell_angle(x, y) * (2*math.pi/16)


    x_new = np.cos(angl) * (((np.power(base, dist + 0.5) - 1) * minimal_cell) / (base-1))
    y_new = np.sin(angl) * (((np.power(base, dist + 0.5) - 1) * minimal_cell) / (base-1))
    x_new *= vorz_x
    y_new *= vorz_y

    return (x_new, y_new)

def get_cell(x, y):
    return [distance(x, y), cell_angle(x, y)]
def get_xy_from_cell(r, a):
    prd = (((np.power(base, r+0.5) - 1) * minimal_cell) / (base - 1))
    return (np.cos(a * (2*math.pi/16)) * prd, np.sin(a * (2*math.pi/16)) * prd)

def dist_between(a, b):
    (x1, y1) = get_xy_from_cell(a[0], a[1])
    (x2, y2) = get_xy_from_cell(b[0], b[1])
    return np.sqrt(np.power(x1 - x2, 2) + np.power(y1 - y2, 2))

# A STAR IMPLEMENTATION
def a_star_search(start, goal):
    openlist = Q.PriorityQueue()
    closedlist = set()
    openlist.put((0, start))
    came_from = {}
    cost_so_far = {}
    came_from[start] = None
    cost_so_far[start] = 0

    while not openlist.empty():
        current = openlist.get()[1]
        #print("current = " + str(current))

        if current == goal:
            break

        closedlist.add(current)

        for the_next in [(current[0], current[1]+1), (current[0], current[1]-1), (current[0]-1, current[1]), (current[0]-1, current[1]+1), (current[0]-1, current[1]-1)]:
            if the_next in closedlist:
                continue
            new_cost = cost_so_far[current] + dist_between(current, the_next)
            if the_next not in cost_so_far or new_cost < cost_so_far[the_next]:
                cost_so_far[the_next] = new_cost
                priority = new_cost + dist_between(the_next, goal) #+ (np.absolute(goal[1] - the_next[1]))
                #print(the_next)
                #print(dist_between(the_next, goal))
                #print("new_cost = " + str(new_cost))
                #print("priority = " + str(priority))
                openlist.put((priority, the_next))
                came_from[the_next] = current

    return came_from, cost_so_far

def compute_waypoints_LPG(tar):
    start = (get_cell(tar[0], tar[1])[0], get_cell(tar[0], tar[1])[1])
    target = (0, 0)

    (a, b) = a_star_search(start, target)
    the_next = (0, 0)
    the_path = [the_next]
    while a[the_next] in a:
        the_next = a[the_next]
        the_path.append(the_next)

    return the_path

def compute_waypoints_LPGown(target):
    return [[0, 0]]
def compute_waypoints_own(target):
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

    # draw rings
    for k in range(1, 17):
        rad = inv_distance(k)
        ax.add_artist(Circle(xy=(0, 0), radius=rad, fill=False, color='black'))

    # draw angular partitions
    for k in range(0, len(x)):
        ax.plot([0, x[k]], [0, y[k]], 'black')

    # draw obstacles
    if (obstacles is not []):
        obstacle_set = set()
        for k in obstacles:
            obstacle_set.add(cell_mid(k[0], k[1]))
        for k in obstacle_set:
            ax.plot(k[0], k[1], '*', c='red')
    # draw target
    ax.plot(cell_mid(target[0], target[1])[0], cell_mid(target[0], target[1])[1], 'x', c='green')

    # draw path
    if algorithm is "LPG":
        waypoints = compute_waypoints_LPG(target)
    if algorithm is "LPGown":
        waypoints = compute_waypoints_LPGown(target)
    if algorithm is "own":
        waypoints = compute_waypoints_own(target)

    # mark waypoint cells
    for k in waypoints:
        (x, y) = get_xy_from_cell(k[0], k[1])
        ax.plot(x, y, ">", c='blue')
        #print(get_xy_from_cell(k[0], k[1]))

    ax.set_xlim([-4500, 4500])
    ax.set_ylim([-3000, 3000])

    plt.pause(1)
