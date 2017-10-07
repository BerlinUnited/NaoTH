from __future__ import division
from matplotlib import pyplot as plt
import numpy as np
import pickle

fixed_rotation = 0
fixed_x = 1000
fixed_y = 100

decisions_1_samples = pickle.load(open("../data/humanoids/decision-simulate_every_pos-30-100.pickle", "rb"))
for pos in decisions_1_samples:
    x, y, rotation, new_decision_histogram = pos
    print(y)
    if rotation == fixed_rotation and x == fixed_x and y == fixed_y:
        print(new_decision_histogram)

        plt.bar(range(0, len(new_decision_histogram)), new_decision_histogram)
        plt.show()
