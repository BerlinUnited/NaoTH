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

obstacles      = [(1600, 0, 300), (2000, -500, 300)]#[(-500, 1500, 300), (-750, 1000, 300)] # [(), ()] ansonsten fehler
target         = [(3500, 0)]#, (-1000, -2750)]
orig_target    = copy.copy(target)
current_target = 0

x_off = 0
y_off = 0

while True:
    mpl.rcParams['lines.linewidth'] = 0.5
    plt.clf()
    plt.axis("equal")
    ax = plt.gca()
    ax.set_xlim([-5500, 5500])
    ax.set_ylim([-3500, 3500])

    # draw the field
    B.draw_field(ax, 0, 0)

    steps = B.compute_steps(target[current_target], orig_target[current_target], obstacles, x_off, y_off)
    B.draw_steps(ax, x_off, y_off, steps)

    # draw obstacles and the target
    B.draw_obstacles(ax, 0, 0, obstacles)
    B.draw_target(ax, orig_target)

    # simulate gait
    if (x_off != target[current_target][0] or y_off != target[current_target][1]) and (len(steps) > 0):
        x_off += math.floor(steps[0][0])
        y_off += math.floor(steps[0][1])
        target[current_target] = (target[current_target][0] - math.floor(steps[0][0]), target[current_target][1] - math.floor(steps[0][1]))
    # change target
    if x_off == orig_target[current_target][0] and y_off == orig_target[current_target][1] and current_target < len(target) - 1:
        current_target        += 1
        target[current_target] = (target[current_target][0] - x_off, target[current_target][1] - y_off)

    plt.pause(0.000000000001)
