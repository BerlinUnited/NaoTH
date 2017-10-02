from __future__ import division
from matplotlib import pyplot as plt
import pickle
import numpy as np
import scipy.io

from tools import tools
"""
 reads pickle files from data/potential_field_generation
 Pickle File format:  x, y, time, angle
"""

gen_field_own = pickle.load(open("../data/potential_field_generation/potential_field_gen_own0.pickle", "rb"))

plt.clf()
tools.draw_field()

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
# np.savetxt('../data/potential_field_generation/f.out', nyi, delimiter=',')   # X is an array

# Matlab Export
# scipy.io.savemat('../data/potential_field_generation/potentials.mat', mdict={'potentials': f-g})

# Plot potentials
plt.pcolor(nxi, nyi, f-g, cmap="Greys_r", alpha=0.8)
plt.show()


"""
# Test contour plot
for own in potentials_own:
    i += 1
    x, y, time, angle = own
    x_own = np.append(x_own, x)  # x_own.append(x)
    y_own = np.append(y_own, y)  # y_own.append(y)
    times_own = np.append(times_own, time)  # times_own.append(time)

# Through the unstructured data get the structured data by interpolation
xi = np.linspace(x_own.min()-1, x_own.max()+1, 100)
yi = np.linspace(y_own.min()-1, y_own.max()+1, 100)
zi = griddata(x_own, y_own, times_own, xi, yi, interp='linear')

CS = plt.contourf(xi, yi, zi, 10, alpha=0.5, cmap="coolwarm", frameon=False)
plt.colorbar()
plt.show()

"""
