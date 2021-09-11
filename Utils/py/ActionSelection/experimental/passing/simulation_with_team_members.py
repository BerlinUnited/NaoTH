import math
import numpy as np
from matplotlib import pyplot as plt
from matplotlib.patches import Circle
from tools import action as a
from tools import Simulation as Sim
from naoth.math import *
from tools import tools

"""
    This script is basically the same as run_simulation.py but it draws other robots as well
    TODO: use the robots positions for changing the decisions
"""
no_action = a.Action("none", 0, 0, 0, 0)
kick_short = a.Action("kick_short", 780, 150, 8.454482265522328, 6.992268841997358)
sidekick_left = a.Action("sidekick_left", 750, 150, 86.170795364136380, 10.669170653645670)
sidekick_right = a.Action("sidekick_right", 750, 150, -89.657943335302260, 10.553726275058064)


class State:
    def __init__(self):
        self.pose = Pose2D()
        self.pose.translation = Vector2(-2900, -2200)
        self.pose.rotation = math.radians(55)

        self.ball_position = Vector2(100.0, 0.0)

        self.obstacle_list = ([])  # is in global coordinates

        self.potential_field_function = "influence_01"

        self.own_robots = [Vector2(-2000, -1000)]
        self.opp_robots = []  # m2d.Vector2(-1800, -1000)

        self.actions = [no_action, kick_short, sidekick_left,
                        sidekick_right]  # possible actions the robot can perform


def draw_actions(actions_consequences, state, best_action):
    plt.clf()
    tools.draw_field()

    axes = plt.gca()
    # plt.gca().set_aspect('equal', adjustable='box')
    # axes.text(0, 0, best_action, fontsize=12)
    axes.add_artist(
        Circle(xy=(state.pose.translation.x, state.pose.translation.y), radius=100, fill=False,
               edgecolor='white'))
    axes.text(-3000, -4500, best_action, fontsize=12)

    # Add the other robots
    for own in state.own_robots:
        axes.add_artist(Circle(xy=(own.x, own.y), radius=100, fill=True, edgecolor='blue'))
    for opp in state.opp_robots:
        axes.add_artist(Circle(xy=(opp.x, opp.y), radius=100, fill=True, edgecolor='red'))

    x = np.array([])
    y = np.array([])

    for consequence in actions_consequences:
        for particle in consequence.positions():
            ball_pos = state.pose * particle.ball_pos  # transform in global coordinates

            x = np.append(x, [ball_pos.x])
            y = np.append(y, [ball_pos.y])

    plt.scatter(x, y, c='r', alpha=0.5)

    axes.text(-4500, -3000, best_action, fontsize=12)

    plt.ylim([-3500, 0])
    plt.xlim([-5000, 0])

    plt.pause(0.0001)


def main():
    state = State()

    """
    no_action = a.Action("none", 0, 0, 0, 0)
    kick_short = a.Action("kick_short", 780, 150, 8.454482265522328, 6.992268841997358)
    sidekick_left = a.Action("sidekick_left", 750, 150, 86.170795364136380, 10.669170653645670)
    sidekick_right = a.Action("sidekick_right", 750, 150, -89.657943335302260, 10.553726275058064)
    """

    action_list = [no_action, kick_short, sidekick_left, sidekick_right]

    while True:
        actions_consequences = []
        # Simulate Consequences
        for action in action_list:
            single_consequence = a.ActionResults([])
            actions_consequences.append(
                Sim.simulate_consequences(action, single_consequence, state, 30))

        # actions_consequences is now a list of ActionResults

        # Decide best action
        best_action = Sim.decide_smart(actions_consequences, state)

        draw_actions(actions_consequences, state, action_list[best_action].name)


if __name__ == "__main__":
    main()
