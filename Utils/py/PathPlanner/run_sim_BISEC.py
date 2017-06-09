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
target       = [-1000, -2750]#[3500, 0]#
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

    (gait, steps, gait_unit_vec) = B.compute_gait(target, obstacles, x_off, y_off)
    #gait = [400, 0]
    B.draw_gait(ax, x_off, y_off, gait, steps, gait_unit_vec)

    # draw obstacles and the target
    B.draw_obstacles(ax, 0, 0, obstacles)
    B.draw_target(ax, orig_target)
    B.draw_robo_pos(ax, (x_off, y_off))

    # simulate gait
    if x_off is not target[0] and y_off is not target[1]:
        x_off += gait[0]
        y_off += gait[1]
        target = (target[0] - gait[0], target[1] - gait[1])

    plt.pause(0.000000000001)
