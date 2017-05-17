from __future__ import division
import sys
import math
import numpy as np
from matplotlib import pyplot as plt
from matplotlib.patches import Circle
import matplotlib as mpl

base           = 1.1789
minimal_cell   = 100
angular_part   = 16

def distance(x, y):
    return math.floor(math.log(((math.sqrt(x*x + y*y) * (base - 1)) / minimal_cell) + 1, base))
def inv_distance(r):
    return (np.exp(np.log(base)*r) - 1) * minimal_cell / (base - 1)

def cell_angle(x, y):
    return math.floor((angular_part / (2*math.pi)) * angle(x, y) + 0.5)
def angle(x,y):
    if (x is 0):
        return np.arctan2(y, 1)
    else:
        return np.arctan2(y, x)
def inv_angle(a):
    result = ((a - 0.5) * (2*math.pi)) / angular_part
    return result

def cell_mid(x, y):
    if (x is not 0):
        vorz_x = np.absolute(x) / x
    else:
        vorz_x = 1
    if (y is not 0):
        vorz_y = np.absolute(y) / y
    else:
        vorz_y = 1
    x = np.absolute(x)
    y = np.absolute(y)

    # distance and angle for the middle of the cell
    dist = distance(x, y)
    angl = cell_angle(x, y) * (2*math.pi/16)


    x_new = np.cos(angl) * (((np.power(base, dist + 0.5) - 1) * minimal_cell) / (base-1))
    y_new = np.sin(angl) * (((np.power(base, dist + 0.5) - 1) * minimal_cell) / (base-1))
    x_new *= vorz_x
    y_new *= vorz_y
    x_new = np.clip(x_new, -4500, 4500)
    y_new = np.clip(y_new, -3000, 3000)

    return (x_new, y_new)

def get_cell(x, y):
    return [distance(x, y), angle(x, y)]

def draw_lpg(obstacles, target):
    if not target:
        print("Need a target!")
        sys.exit()

    a_length = 2*math.pi / angular_part
    radius = 6000
    #a = np.arange(-math.pi/angular_part, math.pi*angular_part, 2*math.pi/angular_part)
    a = (np.arange(0, angular_part) + 0.5) * a_length
    x = np.cos(a) * radius
    y = np.sin(a) * radius

    d = np.arange(0, 4500, 4500/16)

    mpl.rcParams['lines.linewidth'] = 0.5

    plt.clf()
    plt.axis("equal")
    ax = plt.gca()

    # draw rings
    for k in range(1, 17):
        rad = inv_distance(k)
        ax.add_artist(Circle(xy=(0, 0), radius=rad, fill=False, color='black'))

    # draw angular partitions
    for k in range(0, len(x)):
        ax.plot([0, x[k]], [0, y[k]], 'black')

    # draw obstacles and target
    if (obstacles is not []):
        obstacle_set = set()
        for k in obstacles:
            obstacle_set.add(cell_mid(k[0], k[1]))
            for k in obstacle_set:
                ax.plot(k[0], k[1], '*', c='red')
    ax.plot(cell_mid(target[0], target[1])[0], cell_mid(target[0], target[1])[1], 'x', c='green')

    ax.set_xlim([-4500, 4500])
    ax.set_ylim([-3000, 3000])

    plt.pause(0.001)
