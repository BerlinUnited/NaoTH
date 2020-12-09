from __future__ import division
import math
import numpy as np
from matplotlib.patches import Circle
import Queue as Q
import time

base = 1.1789
minimal_cell = 100
angular_part = 16

parameter_s = 0.5

robot_radius = 0


def get_r(coords):
    return math.floor(math.log(((math.sqrt(np.power(coords[0], 2) + np.power(coords[1], 2)) * (
                base - 1)) / minimal_cell) + 1, base))


def get_inv_r(r):
    return (np.exp(np.log(base) * r) - 1) * minimal_cell / (base - 1)


def get_a(coords, rot):
    return math.floor((angular_part / (2 * np.pi)) * (angle(coords) - rot) + 0.5)


def angle(coords):
    if (coords[0] == 0):
        return np.arctan2(coords[1], 1)
    else:
        return np.arctan2(coords[1], coords[0])


def get_cell_mid((r, a), rot):  # returns cell mid from polar coordinates
    prd = (((np.power(base, r + 0.5) - 1) * minimal_cell) / (base - 1))
    return (np.cos(a * (2 * np.pi / 16) + rot) * prd, np.sin(a * (2 * np.pi / 16) + rot) * prd)


def get_cell(coords, rot):
    return (get_r(coords), get_a(coords, rot))


def dist(a, b):
    (x1, y1) = (a[0], a[1])
    (x2, y2) = (b[0], b[1])
    return np.sqrt(np.power(x1 - x2, 2) + np.power(y1 - y2, 2))


def dist_cell(a, b, rot):
    (x1, y1) = get_cell_mid(a, rot)
    (x2, y2) = get_cell_mid(b, rot)
    return np.sqrt(np.power(x1 - x2, 2) + np.power(y1 - y2, 2))


def obst_func(cell, obst, rot):  # obst is obstacle coordinates in x, y
    r_f = obst[2]
    cell_mid = get_cell_mid(cell, rot)
    dist_to_obst_mid = dist(cell_mid, obst)
    obst_dist_to_mid = dist(obst, (0, 0))
    r_d = obst_dist_to_mid / 10

    # parameters
    a = r_f - r_d  # cost of constant part
    r = r_f + r_d  # radius of constant part
    s = parameter_s * r  # radius of linear decreasing part

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
    start = time.time()
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
                new_cost = cost_so_far[current] + dist_cell(current, the_next, rot)

                # add obstacle costs to cell
                for obst in obstacles:
                    new_cost += obst_func(the_next, obst, rot)

                # add to or update openlist
                if the_next not in cost_so_far or new_cost < cost_so_far[the_next]:
                    cost_so_far[the_next] = new_cost
                    priority = new_cost + dist_cell(the_next, goal, rot)
                    openlist.put((priority, the_next))
                    came_from[the_next] = current

                if time.time() - start > 10:
                    return None, None
    return came_from, cost_so_far


def compute_waypoints(tar, obstacles, rot, rot_a):
    start = get_cell(tar, rot)
    target = (0, rot_a)

    (a, b) = a_star_search(start, target, obstacles, rot)

    if a is None:
        return None

    the_next = target
    the_path = [the_next]

    while a[the_next] in a:
        the_next = a[the_next]
        the_path.append(the_next)

    return the_path


def compute_gait(the_path, target, rot):
    (x, y) = (0, 0)
    for k in range(0, len(the_path)):
        (x, y) = get_cell_mid(the_path[k], rot)
        if (np.absolute(x) >= 60) or (np.absolute(y) >= 60):
            break

    distance = dist((x, y), (0, 0))
    max_steplength = min(60, max(-60, distance))
    gait = (x / distance * max_steplength, y / distance * max_steplength)

    if np.sqrt(np.power(gait[0], 2) + np.power(gait[1], 2)) > np.sqrt(
            np.power(target[0], 2) + np.power(target[1], 2)):
        gait = target

    return gait


def draw_LPG(ax, robot_pos, rot):
    a_length = 2 * math.pi / angular_part
    radius = 60000
    a = (np.arange(0, angular_part) + 0.5) * a_length
    x = np.cos(a + rot) * radius
    y = np.sin(a + rot) * radius

    # draw rings
    for k in range(0, 17):
        rad = get_inv_r(k)
        ax.add_artist(
            Circle(xy=(robot_pos[0], robot_pos[1]), radius=rad, fill=False, color='black',
                   alpha=.25))

    # draw angular partitions
    for k in range(0, len(x)):
        ax.plot([0 + robot_pos[0], x[k] + robot_pos[0]], [0 + robot_pos[1], y[k] + robot_pos[1]],
                'black', alpha=.25)


def draw_waypoints(ax, waypoints, robot_pos, rot):
    # draw waypoint cells
    for k in waypoints:
        (way_x, way_y) = get_cell_mid(k, rot)
        ax.plot(way_x + robot_pos[0], way_y + robot_pos[1], ".", c='blue')


def draw_obstacles(ax, robot_pos, obstacles):
    # draw obstacles
    if obstacles:
        for k in obstacles:
            ax.add_artist(Circle(xy=(k[0], k[1]), radius=k[2] + (dist(k, robot_pos) / 10) + (
                        k[2] + (dist(k, robot_pos) / 10) * parameter_s), fill=True, color='blue',
                                 alpha=.25))
            ax.add_artist(Circle(xy=(k[0], k[1]), radius=k[2] + dist(k, robot_pos) / 10, fill=True,
                                 color='red', alpha=.25))
            ax.add_artist(Circle(xy=(k[0], k[1]), radius=10, fill=True, color='black'))
