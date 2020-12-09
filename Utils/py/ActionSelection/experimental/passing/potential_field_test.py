from __future__ import division
from matplotlib import pyplot as plt
from matplotlib.patches import Circle
import numpy as np
from tools import potential_field as pfield
from tools import tools
from naoth.math import *
import matplotlib as mlp

pgf_with_rc_fonts = {
    "font.family": "serif",
    "font.size": 16,
    "legend.fontsize": 16,
    "font.sans-serif": ["DejaVu Sans"],  # use a specific sans-serif font
}
mlp.rcParams.update(pgf_with_rc_fonts)

if __name__ == "__main__":
    # Constants for robot
    velRot = 60  # grad pro second
    velWalk = 200  # mm pro second
    size_x = 4500
    size_y = 3000

    x = np.arange(-size_x, size_x, 10)
    y = np.arange(-size_y, size_y, 10)
    xm, ym = np.meshgrid(x, y)

    x_dim = x.size
    y_dim = y.size
    zm = np.zeros((y_dim, x_dim))

    own_robots = [Vector2(-2000, -2000), Vector2(2000, 1000)]
    opp_robots = [Vector2(0, 1000), Vector2(3000, 2000), Vector2(-1500, -500)]
    # TODO where should the friendly robot be according to the graphic in my thesis
    # own_robots = []
    # opp_robots = []
    for i in range(int(y_dim)):
        for j in range(int(x_dim)):
            zm[i, j] = pfield.evaluate_action_with_robots(Vector2(xm[i][j], ym[i][j]),
                                                          opp_robots, own_robots)

    # plot
    fig = plt.figure(frameon=False)
    ax = fig.gca()
    # tools.draw_field(ax)
    ax.set_aspect("equal")
    ax.set_xlabel("x [mm]")
    ax.set_ylabel("y [mm]")
    ax.axis('on')
    ax.set_xlim([-size_x, size_x])
    ax.set_ylim([-size_y, size_y])
    ax.spines['left'].set_position(('axes', 0.0))
    ax.spines['bottom'].set_position(('axes', 0.0))
    ax.spines['right'].set_visible(False)
    ax.spines['top'].set_visible(False)

    ax.get_xaxis().tick_bottom()  # remove unneeded ticks
    ax.get_yaxis().tick_left()

    CS1 = plt.contourf(x, y, zm, 10, alpha=0.6, cmap="coolwarm", frameon=False)

    CS = plt.contour(CS1, levels=CS1.levels)
    plt.clabel(CS, inline=1, fontsize=10)
    '''
    count1 = 1
    for robot in own_robots:
        ax.add_artist(Circle(xy=(robot.x, robot.y), radius=50, fill=True, color='white'))
        # ax.arrow(robot.x, origin.y, arrow_head.x, arrow_head.y, head_width=100, head_length=100, fc='k', ec='k')
        ax.text(robot.x, robot.y, "own "+str(count1), fontsize=5)
        count1 += 1

    count2 = 1
    for robot in opp_robots:
        ax.add_artist(Circle(xy=(robot.x, robot.y), radius=50, fill=True, color='red'))
        # ax.arrow(robot.x, origin.y, arrow_head.x, arrow_head.y, head_width=100, head_length=100, fc='k', ec='k')
        ax.text(robot.x, robot.y, "opp " + str(count2), fontsize=5)
        count2 += 1
    '''
    plt.show()
