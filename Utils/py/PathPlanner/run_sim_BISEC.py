from __future__ import division
import BISEC as B
from random import uniform as rand
import sys
from matplotlib import pyplot as plt
import matplotlib as mpl
import copy
import select
from sys import argv
import os.path

if len(argv) > 1:
    file_count = 0
    if len(argv) == 2:
        script, filename = argv
    if len(argv) == 3:
        script, filename, max_exp = argv
        max_exp = int(max_exp)

    orig_filename = copy.copy(filename)
    if os.path.isfile(filename):
        file_count = 1
        filename = filename + str(file_count)
        while os.path.isfile(filename):
            file_count += 1
            filename = orig_filename + str(file_count)
    if file_count > 0:
        print("The specified file already exists, renaming to " + filename + ".")
    file = open(filename, 'w')


def heard_pause():
    i, o, e = select.select([sys.stdin], [], [], 0.0001)
    for s in i:
        if s == sys.stdin:
            input = sys.stdin.readline()
            return True
    return False


obstacles = []
target = []
robot_pos = (0, 0)

f_inf = (4500, 3000)

steps = []

pause = False
exp_count = 0

loop_bool = True

while loop_bool:
    if heard_pause():
        pause = not pause
    while pause:
        if heard_pause():
            pause = not pause

    if len(argv) == 3:
        loop_bool = eval('exp_count < max_exp')
        if not loop_bool:
            sys.exit()

    exp_count += 1
    print("Experiment " + str(exp_count) + ".")

    obstacles = []
    target = (rand(-f_inf[0], f_inf[0]), rand(-f_inf[1], f_inf[1]))
    while len(obstacles) < 9:
        obst_r = 300
        obst_x = rand(-f_inf[0], f_inf[0])
        obst_y = rand(-f_inf[1], f_inf[1])
        if obst_x <= (obst_r + B.robot_radius) and obst_x >= -1 * (
                obst_r + B.robot_radius) and obst_y <= (
                obst_r + B.robot_radius) and obst_y >= -1 * (obst_r + B.robot_radius):
            continue
        if B.dist((obst_x, obst_y), target) <= obst_r + B.robot_radius:
            continue
        do_add = True
        if len(obstacles) > 0:
            for k in obstacles:
                if B.dist((obst_x, obst_y), k) <= 2 * k[2]:
                    do_add = False
                    break
        if do_add:
            obstacles.append((obst_x, obst_y, obst_r))

    mpl.rcParams['lines.linewidth'] = 0.5
    plt.clf()
    plt.axis("equal")
    ax = plt.gca()
    ax.set_xlim([-5500, 5500])
    ax.set_ylim([-3500, 3500])

    # draw the field
    B.draw_field(ax, 0, 0)

    steps_plus = B.compute_path(robot_pos, target, obstacles, 0, ax, 1)
    steps_minus = B.compute_path(robot_pos, target, obstacles, 0, ax, -1)

    if len(steps_plus) < len(steps_minus):
        steps = steps_plus
        alt_steps = steps_minus
    else:
        steps = steps_minus
        alt_steps = steps_plus

    # draw the path
    B.draw_steps(ax, robot_pos, alt_steps, 'blue')
    B.draw_steps(ax, robot_pos, steps, 'red')

    # draw obstacles, the target and the roboter
    B.draw_obstacles(ax, obstacles)
    B.draw_target(ax, target)
    B.draw_robot(ax, robot_pos)

    if len(argv) > 1:
        file.write(str(steps) + ", " + str(obstacles) + ", " + str(target) + ", " + str(
            robot_pos) + ", " + str(B.robot_radius))
        file.write("\n")

    pause = False

    plt.pause(0.000000000001)
