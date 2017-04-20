import math
import copy
import numpy as np
from matplotlib import pyplot as plt
from matplotlib.patches import Circle
from tools import action as a
from tools import Simulation as Sim
from naoth import math2d as m2d
from tools import tools
import time


class State:
    def __init__(self):
        self.pose = m2d.Pose2D()
        self.pose.translation = m2d.Vector2(0, 0)
        self.pose.rotation = math.radians(0)

        self.ball_position = m2d.Vector2(100.0, 0.0)

        self.obstacle_list = ([])  # is in global coordinates


def draw_actions(actions_consequences, state, best_action, normal_angle, better_angle, angle_list):
    plt.clf()
    tools.draw_field()

    axes = plt.gca()
    axes.add_artist(Circle(xy=(state.pose.translation.x, state.pose.translation.y), radius=100, fill=False, edgecolor='white'))
    axes.text(0, 0, best_action, fontsize=12)

    origin = state.pose.translation
    # arrow_head = m2d.Vector2(500, 0).rotate(math.radians(normal_angle + better_angle))

    for angle in angle_list:
        arrow_head = m2d.Vector2(500, 0).rotate(state.pose.rotation + math.radians(normal_angle + angle))
        axes.arrow(origin.x, origin.y, arrow_head.x, arrow_head.y, head_width=100, head_length=100, fc='k', ec='k')

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

    num_particles = 30
    num_angle_particle = 30
    angle_list = np.random.randint(low=0, high=360, size=num_angle_particle)
    ####################################################################################################################
    while True:
        # Do normal Simulation
        actions_consequences = []
        # Simulate Consequences
        for action in action_list:
            single_consequence = a.ActionResults([])
            actions_consequences.append(Sim.simulate_consequences(action, single_consequence, state, num_particles))

        # actions_consequences is now a list of ActionResults

        # Decide best action
        best_action = Sim.decide_smart(actions_consequences, state)

        ####################################################################################################################
        # Do it more than once for one simulation cycle
        for i in xrange(0, 20):
            # Modify angle of best action with particle Filter
            bla_num_particles = 1
            improved_action = copy.deepcopy(action_list[best_action])

            improved_actions_consequences = []
            for angle in angle_list:
                # Set the angle for the kick to angle
                improved_action.angle = angle

                # Simulate Consequences
                improved_single_consequence = a.ActionResults([])
                improved_actions_consequences.append(Sim.simulate_consequences(improved_action, improved_single_consequence, state, bla_num_particles))

            # actions_consequences is now a list of ActionResults of one action with different angles

            # Decide best action
            improved_best_action = Sim.decide_smart(improved_actions_consequences, state)
            # print("Best Action: " + str(improved_best_action) + " with angle: " + str(angle_list[improved_best_action]))

            # Resampling - just take the best and get new samples near them + some random sample from a uniform distribution
            mu = angle_list[improved_best_action]  # mean of best angle
            test = (np.random.random(10) - 0.5) * 2  # returns 10 values between -1 and 1
            new_angle_list = (test + mu) % 360
            # new_angle_list = np.append(new_angle_list, np.random.randint(low=0, high=360, size=5))  # Add random particles
            new_angle_list = np.append(new_angle_list, np.repeat(angle_list[improved_best_action], 15))

            angle_list = new_angle_list

        # print(angle_list[improved_best_action])
        draw_actions(actions_consequences, state, action_list[best_action].name, action_list[best_action].angle,  angle_list[improved_best_action], angle_list)


if __name__ == "__main__":
    main()
