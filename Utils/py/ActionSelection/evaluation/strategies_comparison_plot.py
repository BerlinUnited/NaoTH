from __future__ import division
import os, sys, inspect
cmd_subfolder = os.path.realpath(os.path.abspath(os.path.join(os.path.split(inspect.getfile( inspect.currentframe() ))[0],"..")))
if cmd_subfolder not in sys.path:
    sys.path.insert(0, cmd_subfolder)

from matplotlib import pyplot as plt
import pickle
import numpy as np
from tools import tools


"""
 reads data/strategy_times.pickle
 Pickle File format:  x, y, rot, time_current_impl, time_particle_filter
 
 reads data/strategy_actions.pickle
 Pickle File format:  x, y, rot, c_kicks, c_turns, p_kicks, p_turns
"""

data_prefix = "./data/"

actions = pickle.load(open(str(data_prefix) + "strategy_actions-rot0-1.pickle", "rb"))
strategies = pickle.load(open(str(data_prefix) + "strategy_times-rot0-1.pickle", "rb"))

ax = plt.gca()
tools.draw_field(ax)

nx = {}
ny = {}
for pos in strategies:
    x, y, angle, time_old, time_particle = pos
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
    x, y, _, time_old, time_particle = pos
    if np.isnan(time_old) or np.isnan(time_particle):
        f[ny[y], nx[x]] = 100

    else:
        f[ny[y], nx[x]] = (time_particle - time_old) / np.max(np.abs([time_particle,time_old]))
        print(time_particle- time_old, time_particle, time_old)
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

print (f.shape, nyi.shape)
ax.pcolor(nxi, nyi, f, cmap="jet", alpha=0.8, vmin=-1, vmax = 1)
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
