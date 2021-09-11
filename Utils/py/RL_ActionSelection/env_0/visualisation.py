# -- imports --

import math

from matplotlib import pyplot as plt
from matplotlib.patches import Circle
from tools import action as a
from tools.action import Category

from tools import Simulation as Sim
from tools import potential_field as pf
from naoth import math2d as m2d
from tools import tools
from state import State
from tools import field_info as field


# -- actual code --

def draw_field():
    # draw the blank field
    plt.clf()
    tools.draw_field(plt.gca())
    axes = plt.gca()


def draw_position(state, comment=None):
    # draw one given position

    axes = plt.gca()

    x, y = state.position.x, state.position.y

    arrow_head = state.direction * 500
    axes.arrow(x, y, arrow_head.x, arrow_head.y, head_width=100, head_length=100, fc='k', ec='k')

    if comment is not None:
        axes.text(x, y + 100, comment, fontsize=12)
    axes.add_artist(Circle(xy=(x, y), radius=100, fill=True, color="red"))


def draw_line(state_1, state_2):
    # draw connecting line between two states

    axes = plt.gca()

    x_1, x_2 = state_1.position.x, state_2.position.x
    y_1, y_2 = state_1.position.y, state_2.position.y

    plt.plot([x_1, x_2], [y_1, y_2], color="blue")


def draw_action_sequence(sequence, fix_plot=True):
    # draw a sequence of postions
    for i, state in enumerate(sequence):
        draw_position(state)
        if i < len(sequence) - 1:
            draw_line(state, sequence[i + 1])

    for state in sequence:
        draw_position(state)

    plt.show(block=fix_plot)


def draw_field_and_sequence(sequence, fix_plot=True):
    draw_field()
    draw_action_sequence(sequence, fix_plot)


if __name__ == "__main__":
    draw_field()
    draw_position(State(position=m2d.Vector2(4000, 0)))

    plt.show(block=True)
