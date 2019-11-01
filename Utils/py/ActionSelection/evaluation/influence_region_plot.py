from __future__ import division
from matplotlib import pyplot as plt
import numpy as np
import math as m
import matplotlib as mlp

pgf_with_rc_fonts = {
    "font.family": "serif",
    "font.size": 16,
    "legend.fontsize": 16,
    "font.sans-serif": ["DejaVu Sans"],  # use a specific sans-serif font
}
mlp.rcParams.update(pgf_with_rc_fonts)


def estimate_time(x, y):
    angle = np.degrees(np.arctan2(y, x))
    rot_time = np.abs(angle / velRot)

    # calculate the distance
    distance = np.hypot(x, y)
    distance_time = distance / velWalk
    total_time = distance_time + rot_time  # type: np.ndarray

    for d1 in range(len(x)):
        for d2 in range(len(y)):
            total_time[d1, d2] = 1.5 * total_time[d1, d2] * m.exp(-total_time[d1, d2] * 0.1)

            if total_time[d1, d2] >= 5:
                total_time[d1, d2] = 5

            total_time[d1, d2] -= 5

    return total_time


if __name__ == "__main__":
    # Constants for robot
    velRot = 60  # grad pro second
    velWalk = 200  # mm pro second
    size = 1000

    x_val = np.arange(-size, size, 10)
    y_val = np.arange(-size, size, 10)
    xm, ym = np.meshgrid(x_val, y_val)

    times = estimate_time(xm, ym)

    # plot
    fig = plt.figure(frameon=False)
    ax = fig.gca()

    ax.set_aspect("equal")
    ax.set_xlabel("x [mm]")
    ax.set_ylabel("y [mm]")
    ax.axis('on')
    ax.set_xlim([-size, size])
    ax.set_ylim([-size, size])
    ax.spines['left'].set_position(('axes', 0.0))
    ax.spines['bottom'].set_position(('axes', 0.0))
    ax.spines['right'].set_visible(False)
    ax.spines['top'].set_visible(False)

    ax.get_xaxis().tick_bottom()  # remove unneeded ticks
    ax.get_yaxis().tick_left()

    CS1 = plt.contourf(x_val, y_val, times, 10, alpha=0.5, cmap="coolwarm", frameon=False)

    CS = plt.contour(CS1, levels=CS1.levels)
    plt.clabel(CS, inline=1, fontsize=10)

    plt.show()
