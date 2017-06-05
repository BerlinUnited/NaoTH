from __future__ import division
import LPG
from random import randint
import math
import sys
import numpy as np
from matplotlib import pyplot as plt
from matplotlib.patches import Circle
import matplotlib as mpl
import Queue as Q
import copy
import select

def heard_pause():
    i,o,e = select.select([sys.stdin],[],[],0.0001)
    for s in i:
        if s == sys.stdin:
            input = sys.stdin.readline()
            return True
    return False

# LPG parameters
base           = 1.1789
minimal_cell   = 100
angular_part   = 16
parameter_s = 1

obstacles = [(-500, 1500, 300), (-750, 1000, 300)]#[(1600, 400, 300), (2000, -500, 300)] # [(), ()] ansonsten fehler
target    = [-1000, 2750]

x_off = 0
y_off = 0
rot   = np.arctan2(target[1], target[0])
rot_a = LPG.cell_angle(target, rot)

orig_waypoints = LPG.compute_waypoints_LPG(target, obstacles, rot, rot_a)
orig_obstacles = copy.copy(obstacles)
orig_target    = copy.copy(target)

actual_path    = [(0, 0)]


pause = False

# plot
while True:
    if heard_pause():
        pause = not pause
    while pause:
        if heard_pause():
            pause = not pause

    mpl.rcParams['lines.linewidth'] = 0.5
    plt.clf()
    plt.axis("equal")
    ax = plt.gca()

    # plot field
    LPG.draw_field(ax, 0, 0)

    LPG.draw_obstacles(ax, x_off, y_off, orig_obstacles)

    waypoints = LPG.compute_waypoints_LPG(target, obstacles, rot, rot_a)
    LPG.draw_waypoints(ax, waypoints, x_off, y_off, rot)

    LPG.draw_LPG(ax, x_off, y_off, rot)

    # compute next part of actual path
    gait = LPG.compute_gait(waypoints, target, rot)
    actual_path.append((actual_path[len(actual_path)-1][0] + gait[0], actual_path[len(actual_path)-1][1] + gait[1]))

    # simulate the gait
    x_off += gait[0]
    y_off += gait[1]
    rot    = np.arctan2(target[1], target[0])
    rot_a  = LPG.cell_angle(target, rot)
    target[0] -= gait[0]
    target[1] -= gait[1]
    for k in range(0, len(obstacles)):
        obstacles[k] = (obstacles[k][0] - gait[0], obstacles[k][1] - gait[1], obstacles[k][2])

    # draw actual path
    for k in range(0, len(actual_path) - 1):
        ax.plot([actual_path[k][0], actual_path[k+1][0]], [actual_path[k][1], actual_path[k+1][1]], c='red')

    # draw target
    ax.plot(orig_target[0], orig_target[1], 'x', c='red')

    ax.set_xlim([-5500, 5500])
    ax.set_ylim([-3500, 3500])
    plt.pause(0.000000000001)
