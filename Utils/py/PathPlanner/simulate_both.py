from __future__ import division
import LPG
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
from sys import argv
import os.path
import sim
import naiv as N

file, max_exp = sim.open_file(argv)

keys = {"p": False, "d": False, "e": False, "n": False, "r": False, "t": False, "escape": False}
def key_pressed(event):
    keys[str(event.key)] = True
def key_released(event):
    keys[str(event.key)] = False

plt.connect("key_press_event", key_pressed)
#plt.connect("key_release_event", key_released)

# LPG parameters
base         = 1.1789
minimal_cell = 100
angular_part = 16
parameter_s  = 1

LPG_obstacles  = []
obstacles      = []
target         = [0, 0]
robot_pos      = (0, 0)
orig_robot_pos = copy.copy(robot_pos)

rot   = np.arctan2(target[1], target[0]) # LPG
rot_a = LPG.get_a(target, rot)      # LPG

orig_waypoints = LPG.compute_waypoints(target, LPG_obstacles, rot, rot_a)
orig_obstacles = copy.copy(obstacles)
orig_target    = copy.copy(target)

waypoints = []
steps     = []

actual_path_LPG  = [(0, 0)]
actual_path_B    = [(0, 0)]
actual_path_naiv = [(0, 0)]
all_paths_B      = []
all_robot_pos    = []

pause = False

exp_count = 0
algorithm = 1

go_back_e  = False
do_skip_e  = False
do_skip_a  = False
do_restart = False
show_sub   = False
loop_bool  = True

sim_obst = True
if len(argv) > 1:
    if argv[1] is not 'f':
        sim_obst = True
    else:
        sim_obst = False

# plot
while loop_bool:
    mpl.rcParams['lines.linewidth'] = 0.5
    plt.clf()
    plt.axis("equal")
    ax = plt.gca()

    # check for key presses
    if keys["p"] == True:
        if pause:
            print("Unpaused.")
        elif not pause:
            print("Paused.")
        pause = not pause
        keys["p"] = False
    if keys["d"] == True:
        print("Experiment " + str(exp_count) + ".")
        print(orig_target, orig_obstacles)
        keys["d"] = False
    if len(argv) <= 2:
        if keys["e"] == True:
            print("Skipping current experiment.")
            do_skip_e = True
            #algorithm = 1
            keys["e"] = False
        if keys["n"] == True:
            print("Switching algorithm.")
            do_skip_a = True
            keys["n"] = False
        if keys["r"] == True:
            print("Restarting experiment.")
            do_restart = True
            keys["r"] = False
    if keys["t"] == True:
        show_sub = not show_sub
        keys["t"] = False
    if keys["escape"] == True:
        sys.exit()

    # switch algorithm
    if np.absolute(robot_pos[0] - orig_target[0]) < 1 and np.absolute(robot_pos[1] - orig_target[1]) < 1 and algorithm < 3:
        algorithm    += 1
        obstacles     = copy.copy(orig_obstacles)
        LPG_obstacles = copy.copy(orig_obstacles)
        target        = copy.copy(orig_target)
        robot_pos     = (0, 0)
    if do_skip_a:
        algorithm += 1
        if algorithm == 4:
            algorithm = 1
        LPG_obstacles    = copy.copy(orig_obstacles)
        obstacles        = copy.copy(orig_obstacles)
        target           = copy.copy(orig_target)
        robot_pos        = copy.copy(orig_robot_pos)
        actual_path_LPG  = [(0, 0)]
        actual_path_B    = [(0, 0)]
        actual_path_naiv = [(0, 0)]
    # restart current experiment
    if do_restart:
        target           = copy.copy(orig_target)
        robot_pos        = copy.copy(orig_robot_pos)
        obstacles        = copy.copy(orig_obstacles)
        LPG_obstacles    = copy.copy(orig_obstacles)
        actual_path_B    = [(0, 0)]
        actual_path_LPG  = [(0, 0)]
        actual_path_naiv = [(0, 0)]
        do_restart       = False
    # save experiment and start next
    if np.absolute(robot_pos[0] - orig_target[0]) < 1 and np.absolute(robot_pos[1] - orig_target[1]) < 1 and algorithm == 3 or do_skip_e:
        if not do_skip_e:
            exp_count += 1

            if not do_skip_e:
                algorithm = 1

            print("Experiment " + str(exp_count) + ".")

            if exp_count > 1 and len(argv) > 2:
                file.write(str(orig_robot_pos) + ", " + str(orig_target) + ", " + str(orig_obstacles))
                file.write("\n")
                file.write(str(actual_path_LPG))
                file.write("\n")
                file.write(str(actual_path_B))
                file.write("\n")
                file.write(str(actual_path_naiv))
                file.write("\n")
                file.write("\n")

            if len(argv) == 4:
                loop_bool = eval('exp_count < max_exp')
                if not loop_bool:
                    sys.exit()

        robot_pos, orig_robot_pos, target, orig_target, obstacles, orig_obstacles, LPG_obstacles, waypoints, orig_waypoints,actual_path_B, actual_path_LPG, actual_path_naiv = sim.new_experiment(sim_obst)

        # stupid experiment???
        do_skip_e = B.stupid_experiment(robot_pos, target, obstacles)

    # plot field
    sim.draw_field(ax)

    # draw obstacles
    if algorithm == 1:
        LPG.draw_obstacles(ax, robot_pos, obstacles)
    if algorithm > 1:
        B.draw_obstacles(ax, obstacles)

    # LPG
    if algorithm == 1:
        waypoints = LPG.compute_waypoints(target, LPG_obstacles, rot, rot_a)
        LPG.draw_waypoints(ax, waypoints, robot_pos, rot)

        LPG.draw_LPG(ax, robot_pos, rot)

        gait = LPG.compute_gait(waypoints, target, rot)

    # BISEC
    if algorithm == 2:
        (gait, path) = B.get_gait(robot_pos, target, obstacles, 0, ax, show_sub)
        ax.plot([robot_pos[0], target[0]],[robot_pos[1], target[1]], c='black')

    # Naiv
    if algorithm == 3:
        gait = N.get_path(robot_pos, target, obstacles)

    # simulate the gait
    if not pause and not do_skip_a:
        gait, target, robot_pos, rot, rot_a, actual_path_B, actual_path_LPG, actual_path_naiv, obstacles, LPG_obstacles = sim.simulate(gait, target, robot_pos, rot, rot_a, actual_path_B, actual_path_LPG, actual_path_naiv, obstacles, orig_obstacles, LPG_obstacles, algorithm)

    # draw actual path
    sim.draw_path(actual_path_LPG, actual_path_B, actual_path_naiv, ax)

    # draw target and robot
    sim.draw_tar_rob(orig_target, robot_pos, ax)

    do_skip_a = False

    ax.set_xlim([-5500, 5500])
    ax.set_ylim([-3500, 3500])
    plt.pause(0.000000000001)
