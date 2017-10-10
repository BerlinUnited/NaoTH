from __future__ import division
import os, sys, inspect
cmd_subfolder = os.path.realpath(os.path.abspath(os.path.join(os.path.split(inspect.getfile( inspect.currentframe() ))[0],"..")))
if cmd_subfolder not in sys.path:
    sys.path.insert(0, cmd_subfolder)

from matplotlib import pyplot as plt
import pickle
import numpy as np
from tools import tools
from tools import field_info as field
from state import State

"""
 reads data/strategy_times.pickle
 Pickle File format:  x, y, rot, time_current_impl, time_particle_filter
 
 reads data/strategy_actions.pickle
 Pickle File format:  x, y, rot, c_kicks, c_turns, p_kicks, p_turns
"""

data_prefix = "./data/"

actions = pickle.load(open(str(data_prefix) + "strategy_actions-rot180-7.pickle", "rb"))
strategies = pickle.load(open(str(data_prefix) + "strategy_times-rot180-7.pickle", "rb"))

ax = plt.gca()
tools.draw_field(ax)

nx = {}
ny = {}
for pos in strategies:
    x, y, fixed_rot, c_num_turn_degrees, c_num_turn_ball_degrees, c_dist_walked, p_num_turn_degrees, p_num_turn_ball_degrees, p_dist_walked, o_num_turn_degrees, o_num_turn_ball_degrees, o_dist_walked = pos
    nx[x] = x
    ny[y] = y

nxi = np.array(sorted(nx.keys()))
nyi = np.array(sorted(ny.keys()))

for i, v in enumerate(nxi):
    nx[v] = i

for i, v in enumerate(nyi):
    ny[v] = i

f = np.zeros((len(ny), len(nx)))

# visualize the fastest approach
min_time = 0
max_time = 0
for pos in strategies:
    x, y, fixed_rot, c_num_turn_degrees, c_num_turn_ball_degrees, c_dist_walked, p_num_turn_degrees, p_num_turn_ball_degrees, p_dist_walked, o_num_turn_degrees, o_num_turn_ball_degrees, o_dist_walked = pos
    
    value = c_dist_walked
    
    if np.isnan(value):# or np.isnan(time_particle):
        f[ny[y], nx[x]] = 0
    else:
        f[ny[y], nx[x]] = (value)# / np.max(np.abs([time_particle,time_old]))
        print f[ny[y], nx[x]]
        #print(time_particle- time_old, time_particle, time_old)
        '''
        if time_particle - time_old < min_time:
            min_time = time_particle - time_old
        if time_particle - time_old > max_time:
            max_time = time_particle - time_old
        '''
    #    if time_old - time_particle < -10:  # old approach is at least 10 seconds faster
    #        f[ny[y], nx[x]] = 50  # red
    #    elif time_particle - time_old < -10:  # new approach is at least 10 seconds faster
    #        f[ny[y], nx[x]] = 100  # green
    #    else:
    #        f[ny[y], nx[x]] = 0  # blue  # new significant changes

'''
for pos in actions:
    x, y, fixed_rot, c_kicks, c_turns, p_kicks, p_turns, o_kicks, o_turns, = pos
    f[ny[y], nx[x]] = o_turns < c_turns
    print p_turns
'''
x_step = 300
y_step = 300
x_range = range(int(-field.x_length / 2 ), int(field.x_length / 2)+x_step, x_step)
y_range = range(int(-field.y_length / 2 ), int(field.y_length / 2)+y_step, y_step)
	
print (np.max(np.max(f)))
ax.pcolor(x_range, y_range, f, cmap="jet", alpha=0.8) #, vmin=-1, vmax = 1
plt.show()
"""
# visualize which approach takes less kicks
ax = plt.gca()
tools.draw_field(ax)
k = np.zeros((len(ny), len(nx)))
for action in actions:
    x, y, rot, c_kicks, c_turns, p_kicks, p_turns = action
    if c_kicks < p_kicks:
        k[ny[y], nx[x]] = 50  # red
    elif c_kicks > p_kicks:
        k[ny[y], nx[x]] = 100  # green
    else:
        k[ny[y], nx[x]] = 0  # blue - no significant changes
    # print(c_kicks, p_kicks)
ax.pcolor(nxi, nyi, k, cmap="brg", vmin=0, vmax=100)
plt.show()

# visualize which approach takes less turns
ax = plt.gca()
tools.draw_field(ax)
t = np.zeros((len(ny), len(nx)))
for action in actions:
    x, y, _, c_kicks, c_turns, p_kicks, p_turns = action

    if c_turns < p_turns:
        t[ny[y], nx[x]] = 50  # red
    elif c_turns > p_turns:
        t[ny[y], nx[x]] = 100  # green
    else:
        t[ny[y], nx[x]] = 0  # blue - no significant changes
    # print(c_kicks, p_kicks)
ax.pcolor(nxi, nyi, t, cmap="brg", vmin=0, vmax=100)
plt.show()
"""
