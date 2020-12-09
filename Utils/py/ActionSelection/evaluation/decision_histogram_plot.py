from __future__ import division
from matplotlib import pyplot as plt
import numpy as np
import pickle

"""
Shows a bar plot of the decision histogram for one robot position on the field

Example:
    run without any parameters

        $ python decision_histogram_plot.py
"""

# Set file for importing the decisions
decisions = pickle.load(open("../data/humanoids/simulate_every_pos-30-100.pickle", "rb"))

# Set robot position
fixed_rotation = 0
fixed_x = 1000
fixed_y = 100

for pos in decisions:
    x, y, rotation, new_decision_histogram = pos

    # only plot if desired position is found
    if rotation == fixed_rotation and x == fixed_x and y == fixed_y:
        plt.bar(range(0, len(new_decision_histogram)), new_decision_histogram)
        plt.show()
