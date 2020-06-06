from __future__ import division
from matplotlib import pyplot as plt
from matplotlib.mlab import griddata
import numpy as np
import scipy.io
import pickle
import os

from tools import tools

"""
 reads pickle files from data/potential_field_generation
 Pickle File format:  x, y, time, angle
"""


def cleanup_nan_values(generated_field):
    file_idx = 0

    # create a structure for the scalar field
    nx = {}
    ny = {}
    for pos in generated_field:
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

    # create the scalar fields
    for position in generated_field:
        x, y, time, _ = position
        if np.isnan(time):
            f[ny[y], nx[x]] = 100
            g[ny[y], nx[-x]] = 100
        else:
            f[ny[y], nx[x]] = time
            g[ny[y], nx[-x]] = time

    # Fix nan outlier
    dummy_container = []
    for position in generated_field:
        x, y, time, angle = position
        if np.isnan(time):
            # print(x, y, f[ny[y], nx[x]])
            mean_val = 0
            # not the outermost rows and columns
            if 0 < ny[y] < len(ny) - 1 and 0 < nx[x] < len(nx) - 1:
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
            if ny[y] is 0 and 0 < nx[x] < len(nx) - 1:
                mean_val += f[ny[y], nx[x] - 1]
                mean_val += f[ny[y], nx[x] + 1]
                mean_val += f[ny[y] + 1, nx[x] - 1]
                mean_val += f[ny[y] + 1, nx[x]]
                mean_val += f[ny[y] + 1, nx[x] + 1]
                mean_val /= 5
            # top row except corners
            if ny[y] is len(ny) - 1 and 0 < nx[x] < len(nx) - 1:
                mean_val += f[ny[y] - 1, nx[x] - 1]
                mean_val += f[ny[y] - 1, nx[x]]
                mean_val += f[ny[y] - 1, nx[x] + 1]
                mean_val += f[ny[y], nx[x] - 1]
                mean_val += f[ny[y], nx[x] + 1]
                mean_val /= 5
            # left column except borders
            if 0 < ny[y] < len(ny) - 1 and nx[x] is 0:
                mean_val += f[ny[y] - 1, nx[x]]
                mean_val += f[ny[y] - 1, nx[x] + 1]
                mean_val += f[ny[y], nx[x] + 1]
                mean_val += f[ny[y] + 1, nx[x]]
                mean_val += f[ny[y] + 1, nx[x] + 1]
                mean_val /= 5
            # right column except borders
            if 0 < ny[y] < len(ny) - 1 and nx[x] is len(nx) - 1:
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
            if ny[y] is len(ny) - 1 and nx[x] is len(nx) - 1:
                mean_val += f[ny[y] - 1, nx[x] - 1]
                mean_val += f[ny[y] - 1, nx[x]]
                mean_val += f[ny[y], nx[x] - 1]
                mean_val /= 3
            if ny[y] is 0 and nx[x] is len(nx) - 1:
                mean_val += f[ny[y], nx[x] - 1]
                mean_val += f[ny[y] + 1, nx[x] - 1]
                mean_val += f[ny[y] + 1, nx[x]]
                mean_val /= 3
            if ny[y] is len(ny) - 1 and nx[x] is 0:
                mean_val += f[ny[y] - 1, nx[x]]
                mean_val += f[ny[y] - 1, nx[x] + 1]
                mean_val += f[ny[y], nx[x] + 1]
                mean_val /= 3

            f[ny[y], nx[x]] = mean_val
            g[ny[y], nx[-x]] = mean_val

            dummy_container.append([x, y, mean_val, angle])
        else:
            dummy_container.append([x, y, time, angle])

    # TODO overwrite existing file instead of creating a new one at each run
    # while os.path.exists('{}{:d}.pickle'.format('../data/potential_field_generation/clean_field_v', file_idx)):
    #     file_idx += 1
    # pickle.dump(dummy_container, open('../data/potential_field_generation/clean_field_v' + str(file_idx) + '.pickle', "wb"))

    return dummy_container


def plot_combined_scalar_field(cleaned_field):
    # create a structure for the scalar field
    nx = {}
    ny = {}
    for pos in cleaned_field:
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

    # create the scalar fields
    for position in cleaned_field:
        x, y, time, _ = position
        f[ny[y], nx[x]] = time
        g[ny[y], nx[-x]] = time

    # Plot potentials
    plt.clf()
    tools.draw_field(plt.gca())
    plt.pcolor(nxi, nyi, f - g, cmap="Greys_r", alpha=0.8, zorder=100)
    plt.show()


def contour_plot(cleaned_field):
    i = 0
    x_own = ([])
    y_own = ([])
    times_own = ([])
    for pos in cleaned_field:
        i += 1
        x, y, time, angle = pos
        x_own = np.append(x_own, x)
        y_own = np.append(y_own, y)
        times_own = np.append(times_own, time)

    # reverse the times
    test_reversed = list(reversed(times_own))

    # Through the unstructured data get the structured data by interpolation
    xi = np.linspace(x_own.min() - 1, x_own.max() + 1, 100)
    yi = np.linspace(y_own.min() - 1, y_own.max() + 1, 100)
    zi = griddata(x_own, y_own, times_own - test_reversed, xi, yi, interp='linear')

    plt.clf()
    tools.draw_field(plt.gca())
    plt.contourf(xi, yi, zi, 10, alpha=0.5, cmap="coolwarm", frameon=False, zorder=100)
    plt.colorbar()
    plt.show()


def export_strategies(cleaned_field):
    # Load data:
    # create a structure for the scalar field
    nx = {}
    ny = {}
    for pos in cleaned_field:
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

    # create the scalar fields
    for position in cleaned_field:
        x, y, time, _ = position
        f[ny[y], nx[x]] = time
        g[ny[y], nx[-x]] = time

    # Export for Heinrich
    np.savetxt('../data/potential_field_generation/f.out', nyi, delimiter=',')  # X is an array

    # Matlab Export
    scipy.io.savemat('../data/potential_field_generation/potentials.mat',
                     mdict={'potentials': f - g})


if __name__ == "__main__":
    data_prefix = "D:/RoboCup/Paper-Repos/Bachelor-Schlotter/data/"
    gen_field = pickle.load(
        open(str(data_prefix) + "potential_field_generation/potential_field_gen_own1.pickle",
             "rb"))

    clean_field = cleanup_nan_values(gen_field)

    # plot_combined_scalar_field(clean_field)

    contour_plot(clean_field)

    # export_strategies(clean_field)
