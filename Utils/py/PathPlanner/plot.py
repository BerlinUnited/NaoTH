import numpy as np
from matplotlib import pyplot as plt
from matplotlib.patches import Circle
import sim

all = np.load("current.npy")

targets = all[0]
LPG = all[1]
B = all[2]
naiv = all[3]
obstacles = all[4]

for k in range(0, len(targets)):
    ax = plt.gca()
    ax.set_xlim([-5500, 5500])
    ax.set_ylim([-3500, 3500])

    sim.draw_field(ax)

    for o in obstacles[k]:
        ax.add_artist(
            Circle(xy=(o[0], o[1]), radius=o[2], fill=True, color='black', alpha=.25, zorder='10'))
        ax.add_artist(Circle(xy=(o[0], o[1]), radius=10, fill=True, color='black'))

    ax.plot(targets[k][0], targets[k][1], 'x', c='red')

    for l in range(0, len(naiv[k]) - 1):
        ax.plot([naiv[k][l][0], naiv[k][l + 1][0]], [naiv[k][l][1], naiv[k][l + 1][1]], c='orange')

    for l in range(0, len(LPG[k]) - 1):
        ax.plot([LPG[k][l][0], LPG[k][l + 1][0]], [LPG[k][l][1], LPG[k][l + 1][1]], c='red')

    for l in range(0, len(B[k]) - 1):
        ax.plot([B[k][l][0], B[k][l + 1][0]], [B[k][l][1], B[k][l + 1][1]], c='black')

    plt.show()
