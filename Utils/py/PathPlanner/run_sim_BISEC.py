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

obstacles    = [(1600, 0, 300), (2000, -500, 300)]#[(-500, 1500, 300), (-750, 1000, 300)] # [(), ()] ansonsten fehler
target       = [3500, 0]#[-1000, -2750]#
orig_target  = copy.copy(target)

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

    steps = B.compute_steps(target, obstacles, x_off, y_off)
    B.draw_steps(ax, x_off, y_off, steps)

    # draw obstacles and the target
    B.draw_obstacles(ax, 0, 0, obstacles)
    B.draw_target(ax, orig_target)

    # simulate gait
    if (x_off is not target[0] or y_off is not target[1]) and (len(steps) > 1):
        x_off += steps[1][0]
        y_off += steps[1][1]
        target = (target[0] - steps[1][0], target[1] - steps[1][1])

    plt.pause(0.000000000001)
