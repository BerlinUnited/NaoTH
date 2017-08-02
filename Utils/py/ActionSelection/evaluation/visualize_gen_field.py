from __future__ import division
from matplotlib import pyplot as plt
import pickle
import numpy as np
import scipy.io

from tools import tools
"""
 should read in pickle files from data/potential_field_generation
 Pickle File format:  x, y, time, angle
"""

# gen_field = pickle.load(open("../data/potential_field_generation/potential_field_gen_own0.pickle", "rb"))
gen_field_own = pickle.load(open("../data/potential_field_generation/potential_field_gen_own1.pickle", "rb"))

plt.clf()
tools.draw_field()
axes = plt.gca()
patches = []

nx = {}
ny = {}
for pos in gen_field_own:
    x, y, time, angle = pos
    nx[x] = x
    ny[y] = y


nxi = np.array(sorted(nx.keys()))
nyi = np.array(sorted(ny.keys()))

for i, v in enumerate(nxi):
    nx[v] = i

for i, v in enumerate(nyi):
    ny[v] = i

f = np.zeros((len(ny), len(nx)))
g = np.zeros((len(ny), len(nx)))
for pos in gen_field_own:
    x, y, time, _ = pos
    if np.isnan(time):
        f[ny[y], nx[x]] = 100
        g[ny[y], nx[-x]] = 100
    else:
        f[ny[y], nx[x]] = time
        g[ny[y], nx[-x]] = time

# Fix nan outlier
for pos in gen_field_own:
    x, y, time, _ = pos
    if np.isnan(time):
        print(x, y, f[ny[y], nx[x]])
        mean_val = 0
        # not the outermost rows and columns
        if 0 < ny[y] < len(ny)-1 and 0 < nx[x] < len(nx)-1:
            mean_val += f[ny[y] - 1, nx[x] - 1]
            mean_val += f[ny[y] - 1, nx[x]]
            mean_val += f[ny[y] - 1, nx[x] + 1]
            mean_val += f[ny[y], nx[x] - 1]
            mean_val += f[ny[y], nx[x] + 1]
            mean_val += f[ny[y] + 1, nx[x] - 1]
            mean_val += f[ny[y] + 1, nx[x]]
            mean_val += f[ny[y] + 1, nx[x] + 1]
            mean_val /= 8
        # bottom row except corners
        if ny[y] is 0 and 0 < nx[x] < len(nx)-1:
            mean_val += f[ny[y], nx[x] - 1]
            mean_val += f[ny[y], nx[x] + 1]
            mean_val += f[ny[y] + 1, nx[x] - 1]
            mean_val += f[ny[y] + 1, nx[x]]
            mean_val += f[ny[y] + 1, nx[x] + 1]
            mean_val /= 5
        # top row except corners
        if ny[y] is len(ny)-1 and 0 < nx[x] < len(nx)-1:
            mean_val += f[ny[y] - 1, nx[x] - 1]
            mean_val += f[ny[y] - 1, nx[x]]
            mean_val += f[ny[y] - 1, nx[x] + 1]
            mean_val += f[ny[y], nx[x] - 1]
            mean_val += f[ny[y], nx[x] + 1]
            mean_val /= 5
        # left column except borders
        if 0 < ny[y] < len(ny)-1 and nx[x] is 0:
            mean_val += f[ny[y] - 1, nx[x]]
            mean_val += f[ny[y] - 1, nx[x] + 1]
            mean_val += f[ny[y], nx[x] + 1]
            mean_val += f[ny[y] + 1, nx[x]]
            mean_val += f[ny[y] + 1, nx[x] + 1]
            mean_val /= 5
        # right column except borders
        if 0 < ny[y] < len(ny) - 1 and nx[x] is len(nx)-1:
            mean_val += f[ny[y] - 1, nx[x] - 1]
            mean_val += f[ny[y] - 1, nx[x]]
            mean_val += f[ny[y], nx[x] - 1]
            mean_val += f[ny[y] + 1, nx[x] - 1]
            mean_val += f[ny[y] + 1, nx[x]]
            mean_val /= 5
        # Corners
        if ny[y] is 0 and nx[x] is 0:
            mean_val += f[ny[y], nx[x] + 1]
            mean_val += f[ny[y] + 1, nx[x]]
            mean_val += f[ny[y] + 1, nx[x] + 1]
            mean_val /= 3
        if ny[y] is len(ny)-1 and nx[x] is len(nx)-1:
            mean_val += f[ny[y] - 1, nx[x] - 1]
            mean_val += f[ny[y] - 1, nx[x]]
            mean_val += f[ny[y], nx[x] - 1]
            mean_val /= 3
        if ny[y] is 0 and nx[x] is len(nx)-1:
            mean_val += f[ny[y], nx[x] - 1]
            mean_val += f[ny[y] + 1, nx[x] - 1]
            mean_val += f[ny[y] + 1, nx[x]]
            mean_val /= 3
        if ny[y] is len(ny)-1 and nx[x] is 0:
            mean_val += f[ny[y] - 1, nx[x]]
            mean_val += f[ny[y] - 1, nx[x] + 1]
            mean_val += f[ny[y], nx[x] + 1]
            mean_val /= 3

        f[ny[y], nx[x]] = mean_val
        g[ny[y], nx[-x]] = mean_val
        print(x, y, f[ny[y], nx[x]])


# Export for Heinrich
# np.savetxt('f.out', nyi, delimiter=',')   # X is an array
# Test Import
# f2 = np.loadtxt('f.out', delimiter=',')

# Matlab Export
# scipy.io.savemat('../data/potential_field_generation/potentials.mat', mdict={'potentials': f-g})

# Plot potentials
plt.pcolor(nxi, nyi, f-g, cmap="Greys_r", alpha=0.8)
plt.show()
