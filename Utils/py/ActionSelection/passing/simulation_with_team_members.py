import math
import numpy as np
from matplotlib import pyplot as plt
from matplotlib.patches import Circle
from tools import action as a
from tools import Simulation as Sim
from naoth import math2d as m2d
from tools import tools

"""
    This script is basically the same as run_simulation.py but it draws other robots as well
    TODO: use the robots positions for changing the decisions
"""


class State:
    def __init__(self):
        self.pose = m2d.Pose2D()
        self.pose.translation = m2d.Vector2(-2900, -2200)
        self.pose.rotation = math.radians(45)

        self.ball_position = m2d.Vector2(100.0, 0.0)

        self.obstacle_list = ([])  # is in global coordinates
        # TODO introduce a robot_list
        robot_list = []


def draw_actions(actions_consequences, state, best_action):
    plt.clf()
    tools.draw_field()

    axes = plt.gca()
    # axes.text(0, 0, best_action, fontsize=12)
    axes.add_artist(Circle(xy=(state.pose.translation.x, state.pose.translation.y), radius=100, fill=False, edgecolor='white'))
    axes.text(0, 0, best_action, fontsize=12)
    # Add the other robots
    axes.add_artist(Circle(xy=(-2700, -1000), radius=100, fill=True, edgecolor='blue'))

    x = np.array([])
    y = np.array([])

    for consequence in actions_consequences:
        for particle in consequence.positions():
            ball_pos = state.pose * particle.ball_pos  # transform in global coordinates

            x = np.append(x, [ball_pos.x])
            y = np.append(y, [ball_pos.y])

    plt.scatter(x, y, c='r', alpha=0.5)
    plt.pause(0.0001)


def main():
    state = State()

    no_action = a.Action("none", 0, 0, 0, 0)
    kick_short = a.Action("kick_short", 780, 150, 8.454482265522328, 6.992268841997358)
    sidekick_left = a.Action("sidekick_left", 750, 150, 86.170795364136380, 10.669170653645670)
    sidekick_right = a.Action("sidekick_right", 750, 150, -89.657943335302260, 10.553726275058064)

    action_list = [no_action, kick_short, sidekick_left, sidekick_right]

    while True:
        actions_consequences = []
        # Simulate Consequences
        for action in action_list:
            single_consequence = a.ActionResults([])
            actions_consequences.append(Sim.simulate_consequences(action, single_consequence, state, 30))

        # actions_consequences is now a list of ActionResults

        # Decide best action
        best_action = Sim.decide_smart(actions_consequences, state)

        draw_actions(actions_consequences, state, action_list[best_action].name)


if __name__ == "__main__":
    main()
