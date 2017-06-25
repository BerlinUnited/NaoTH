import numpy as np
from sys import argv
script, filename = argv

file = open(filename, 'r')
all = np.load(file)

robot_pos  = all[0]
targets    = all[1]
obstacles  = all[2]
paths_LPG  = all[3]
paths_B    = all[4]
paths_naiv = all[5]
