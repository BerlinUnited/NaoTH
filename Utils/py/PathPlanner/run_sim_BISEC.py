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

obstacles      = [(1600, 1000, 300), (1600, 500, 300), (1600, 0, 300), (1600, -500, 300)] # [(), ()] ansonsten fehler
target         = (3500, 0)#(3500, 0)]#, (-1000, -2750)]

robot_pos = (0, 0)#(1600, -900)#(0, 0)

steps       = []

pause = False

while True:
    mpl.rcParams['lines.linewidth'] = 0.5
    plt.clf()
    plt.axis("equal")
    ax = plt.gca()
    ax.set_xlim([-5500, 5500])
    ax.set_ylim([-3500, 3500])

    # draw the field
    B.draw_field(ax, 0, 0)

    steps_plus  = B.compute_path(robot_pos, target, obstacles, 0, ax, 1)
    steps_minus = B.compute_path(robot_pos, target, obstacles, 0, ax, -1)

    if len(steps_plus) < len(steps_minus):
        steps     = steps_plus
        alt_steps = steps_minus
    else:
        steps     = steps_minus
        alt_steps = steps_plus

    # draw the path
    B.draw_steps(ax, robot_pos, alt_steps, 'black')
    B.draw_steps(ax, robot_pos, steps, 'red')

    # draw obstacles and the target
    B.draw_obstacles(ax, obstacles)
    B.draw_target(ax, target)

    plt.pause(0.000000000001)
