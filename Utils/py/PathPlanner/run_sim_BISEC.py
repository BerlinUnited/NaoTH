from __future__ import division
import BISEC as B
from random import uniform as rand
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

#obstacles = [(1600, 1000, 450), (1600, 500, 450), (1600, 0, 450), (1600, -500, 450)] # [(), ()] ansonsten fehler
#target    = (3500, 0)#(3500, 0)]#, (-1000, -2750)]

obstacles = []
target    = []
robot_pos = (0, 0)

f_inf     = (4500, 3000)

steps     = []

pause = False

while True:
    if heard_pause():
        pause = not pause
    while pause:
        if heard_pause():
            pause = not pause

    obstacles = []
    target    = (rand(-f_inf[0], f_inf[0]), rand(-f_inf[1], f_inf[1]))
    #robot_pos = (rand(-f_inf[0], f_inf[0]), rand(-f_inf[1], f_inf[1]))
    while len(obstacles) < 9:
        obst_r = 300
        obst_x = rand(-f_inf[0], f_inf[0])
        obst_y = rand(-f_inf[1], f_inf[1])
        if obst_x <= (obst_r + B.robot_radius) and obst_x >= -1*(obst_r + B.robot_radius) and obst_y <= (obst_r + B.robot_radius) and obst_y >= -1*(obst_r + B.robot_radius):
            continue
        if B.dist((obst_x, obst_y), target) <= obst_r + B.robot_radius:
            continue
        obstacles.append((obst_x, obst_y, obst_r))

    print((target, robot_pos))
    print(obstacles)

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
    B.draw_steps(ax, robot_pos, alt_steps, 'blue')
    B.draw_steps(ax, robot_pos, steps, 'red')

    # draw obstacles, the target and the roboter
    B.draw_obstacles(ax, obstacles)
    B.draw_target(ax, target)
    B.draw_robot(ax, robot_pos)

    pause = True

    plt.pause(0.000000000001)
