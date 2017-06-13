from __future__ import division
import BISEC as B
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

obstacles      = [(1600, 250, 300), (2000, -500, 300), (2500, -500, 300)] # [(), ()] ansonsten fehler
target         = [(3500, 0)]#(3500, 0)]#, (-1000, -2750)]
orig_target    = copy.copy(target)

x_off = 0
y_off = 0

obstacle_hit = -1

walked_path = [(x_off, y_off)]

pause = False

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
    ax.set_xlim([-5500, 5500])
    ax.set_ylim([-3500, 3500])

    # draw the field
    B.draw_field(ax, 0, 0)

    (steps, obstacle_hit) = B.compute_steps(target[0], obstacles, x_off, y_off)
    if obstacle_hit > -1:
        target[0]   = orig_target[0]
        sub_target  = B.compute_sub_target(obstacles[obstacle_hit], target[0], x_off, y_off)
        orig_target = [sub_target] + orig_target
        sub_target  = (sub_target[0] - x_off, sub_target[1] - y_off)
        target      = [sub_target] + target

        if len(target) > 2:
            target.pop(1)
            orig_target.pop(1)

    B.draw_steps(ax, x_off, y_off, steps)

    # draw obstacles and the target
    B.draw_obstacles(ax, 0, 0, obstacles)
    B.draw_target(ax, orig_target)

    # draw the walked path
    B.draw_walked_path(ax, walked_path)

    # simulate gait
    if (x_off != target[0][0] or y_off != target[0][1]) and len(steps) > 0 and obstacle_hit == -1:
        x_off += math.floor(steps[0][0])
        y_off += math.floor(steps[0][1])
        target[0] = (target[0][0] - math.floor(steps[0][0]), target[0][1] - math.floor(steps[0][1]))
        walked_path.append((x_off, y_off))
    # change target
    if np.absolute(x_off - orig_target[0][0]) < 1 and np.absolute(y_off - orig_target[0][1]) < 1 and len(target) > 1:
        target.pop(0)
        orig_target.pop(0)
        target[0] = (target[0][0] - x_off, target[0][1] - y_off)

    plt.pause(0.000000000001)
