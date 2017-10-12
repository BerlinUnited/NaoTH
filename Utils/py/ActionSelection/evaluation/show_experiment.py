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


    
if __name__ == "__main__":    

    data_prefix = "./data/"

    experiment = pickle.load(open(str(data_prefix) + "simulation_0.pickle", "rb"))
    
    print experiment['frames'][0]['sim']['optimal_one'][1].turn_around_ball
