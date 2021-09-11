import math
import numpy as np
from matplotlib import pyplot as plt
from tools import field_info as field
from naoth.math import *
from tools import potential_field as pf
from tools import tools
from state import State

if __name__ == "__main__":
    state = State()
    state.opp_robots.append(Pose2D(Vector2(2000, 1000), math.radians(0)))
    state.own_robots.append(Pose2D(Vector2(-2000, 1000), math.radians(0)))

    plt.clf()

    x_val = np.arange(-field.x_field_length / 2, field.x_field_length / 2, 10)
    y_val = np.arange(-field.y_field_length / 2, field.y_field_length / 2, 10)
    potentials = np.zeros((len(y_val), len(x_val)))

    # There is probably a better implementation for this
    step_x = 0
    step_y = 0
    for x in x_val:
        for y in y_val:
            # potentials[step_y][step_x] = pf.evaluate_single_pos(m2d.Vector2(x, y))
            potentials[step_y][step_x] = pf.evaluate_single_pos_with_robots(Vector2(x, y),
                                                                            state.opp_robots,
                                                                            state.own_robots)
            step_y += 1
        step_y = 0
        step_x += 1

    CS1 = plt.contourf(x_val, y_val, potentials, 10, alpha=1, cmap="coolwarm", frameon=False)

    CS = plt.contour(CS1, levels=CS1.levels, zorder=81)
    plt.clabel(CS, inline=True, fontsize=10, colors="black")
    tools.draw_field(plt.gca())
    plt.show()
