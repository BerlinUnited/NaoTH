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


def unpack_data(data):
    s = np.array(data)
    # NOTE: order of fields
    #x, y, fixed_rot, c_num_turn_degrees, c_num_turn_ball_degrees, c_dist_walked, p_num_turn_degrees, p_num_turn_ball_degrees, p_dist_walked, o_num_turn_degrees, o_num_turn_ball_degrees, o_dist_walked

    size_x = np.unique(s[:,0]).shape[0]
    size_y = np.unique(s[:,1]).shape[0]
    shape = (size_x, size_y)
    print("size: {0} x {1}".format(size_x, size_y))

    return [np.transpose(np.reshape(s[:,i], shape)) for i in range(0,s.shape[1])]

    
def get_time(rot, rot_ball, dist):
    state = State()
    return dist / state.walking_vel + rot / state.rotation_vel + rot_ball / state.rotation_vel *2.0
    
    
def plot_data_matrix(f):
    # plot
    fig, axes = plt.subplots(len(f), len(f[0]))

    for i in range(0,len(f)):
        for j in range(0,len(f[i])):
            if len(f[0]) == 1:
                ax = axes[i]
            elif len(f) == 1:
                ax = axes[j]
            else:
                ax = axes[i][j]
            v = f[i][j]
            
            tools.draw_field(ax)

            x_range = np.arange(0,v.shape[1]+1)*(field.x_length/v.shape[1]) - field.x_length/2
            y_range = np.arange(0,v.shape[0]+1)*(field.y_length/v.shape[0]) - field.y_length/2
            
            v[np.isnan(v)] = 0
            ax.pcolor(x_range, y_range, v, cmap="jet", alpha=0.8)

    plt.show()
    
    
if __name__ == "__main__":    

    data_prefix = "./data/"

    #actions = pickle.load(open(str(data_prefix) + "strategy_actions-rot180-8.pickle", "rb"))
    strategies = pickle.load(open(str(data_prefix) + "strategy_times-rot180-8.pickle", "rb"))

    #actions9 = pickle.load(open(str(data_prefix) + "strategy_actions-rot180-9.pickle", "rb"))
    strategies9 = pickle.load(open(str(data_prefix) + "strategy_times-rot180-9.pickle", "rb"))


    x, y, rot, c_turn, c_turn_ball, c_walked, _, _, _, o_turn, o_turn_ball, o_walked = unpack_data(strategies)

    x, y, rot, _, _, _, p_turn, p_turn_ball, p_walked, _, _, _ = unpack_data(strategies9)


    c_time = get_time(c_turn, c_turn_ball, c_walked)
    p_time = get_time(p_turn, p_turn_ball, p_walked)
    o_time = get_time(o_turn, o_turn_ball, o_walked)

    f = [[c_turn_ball, o_turn_ball, p_turn_ball], 
         [c_time, o_time, p_time],
         [c_time > o_time, o_time < p_time, o_turn_ball < p_turn_ball ]]

    plot_data_matrix(f)


