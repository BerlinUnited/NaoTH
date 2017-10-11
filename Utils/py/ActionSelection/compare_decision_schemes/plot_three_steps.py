import math
import numpy as np
import copy
from matplotlib import pyplot as plt
from matplotlib.patches import Circle
from tools import action as a
from tools import Simulation as Sim
from naoth import math2d as m2d
from tools import tools
from tools import raw_attack_direction_provider as attack_dir

from run_simulation_with_particleFilter import calculate_best_direction as heinrich_test

"""
Visualization of Action Selection Algorithm until a goal is scored, with extra variations not considered in Action Model
definition.

Example:
    run without any parameters

        $ python simulate_variate_until_goal.py
"""


class State:
    def __init__(self):
        self.pose = m2d.Pose2D()
        self.pose.translation = m2d.Vector2(-2000, -700)
        self.pose.rotation = math.radians(45)

        self.ball_position = m2d.Vector2(100.0, 0.0)

        # Possible options: normal, influence_01, generated
        self.potential_field_function = "normal"

        self.opp_robots = ([])  # is in global coordinates
        self.own_robots = ([])  # is in global coordinates

        self.next_action = 0

    def update_pos(self, glob_pos, rotation):
        self.pose.translation = glob_pos
        self.pose.rotation = math.radians(rotation)


def draw_robot_walk_lines(axes, line, position_color):
    previous_state = 0
    for rec_state in line:
        origin = rec_state.pose.translation

        # ball_pos = state.pose * state.ball_position

        axes.add_artist(Circle(xy=(origin.x, origin.y), radius=100, fill=False, edgecolor='white'))
        # axes.add_artist(Circle(xy=(ball_pos.x, ball_pos.y), radius=120, fill=True, color=position_color))

        if previous_state != 0:
            axes.arrow(previous_state.x, previous_state.y, origin.x - previous_state.x, origin.y-previous_state.y,
                       head_width=100, head_length=100, color=position_color)

        previous_state = origin


def simulate_goal_cycle_current_impl(iterations):
    state = State()
    sim_data = [copy.deepcopy(state)]

    no_action = a.Action("none", 0, 0, 0, 0)
    kick_short = a.Action("kick_short", 1080, 150, 0, 7)
    sidekick_left = a.Action("sidekick_left", 750, 150, 90, 10)
    sidekick_right = a.Action("sidekick_right", 750, 150, -90, 10)

    action_list = [no_action, kick_short, sidekick_left, sidekick_right]

    # while not goal_scored:
    # do a fixed number of steps
    for i in range(iterations):
        actions_consequences = []
        # Simulate Consequences
        for action in action_list:
            single_consequence = a.ActionResults([])
            actions_consequences.append(Sim.simulate_consequences(action, single_consequence, state, 30))

        # Decide best action
        best_action = Sim.decide_smart(actions_consequences, state)

        # state.next_action = best_action #not next but last action
        sim_data[len(sim_data)-1].next_action = best_action

        # expected_ball_pos should be in local coordinates for rotation calculations
        expected_ball_pos = actions_consequences[best_action].expected_ball_pos_mean

        if not action_list[best_action].name == "none":
            # update the robots position
            rotation = np.arctan2(expected_ball_pos.y, expected_ball_pos.x)
            # print(math.degrees(rotation))
            state.update_pos(state.pose * expected_ball_pos, math.degrees(state.pose.rotation) + rotation)

            # reset the rotation direction
            chosen_rotation = 'none'

            sim_data.append(copy.deepcopy(state))

        elif action_list[best_action].name == "none":
            turn_rotation_step = 5
            # Calculate rotation time
            attack_direction = attack_dir.get_attack_direction(state)

            if (attack_direction > 0 and chosen_rotation) is 'none' or chosen_rotation is 'left':
                state.update_pos(state.pose.translation, math.degrees(state.pose.rotation) + turn_rotation_step)  # Should turn right
                chosen_rotation = 'left'
            elif (attack_direction <= 0 and chosen_rotation is 'none') or chosen_rotation is 'right':
                state.update_pos(state.pose.translation, math.degrees(state.pose.rotation) - turn_rotation_step)  # Should turn left
                chosen_rotation = 'right'

            sim_data.append(copy.deepcopy(state))

    return sim_data


def simulate_goal_cycle_particle(iterations=4, use_sidekicks=True):
    state = State()
    sim_data = [copy.deepcopy(state)]

    no_action = a.Action("none", 0, 0, 0, 0)
    kick_short = a.Action("kick_short", 1080, 150, 0, 7)
    sidekick_left = a.Action("sidekick_left", 750, 150, 90, 10)
    sidekick_right = a.Action("sidekick_right", 750, 150, -90, 10)

    action_list = [no_action, kick_short]
    if use_sidekicks:
        action_list += [sidekick_left, sidekick_right]

    for i in range(iterations):
        # Change Angle of all actions according to the particle filter
        # best_dir is the global rotation for that kick
        best_dir = 360
        best_action = 0
        for ix, action in enumerate(action_list):
            if action.name is "none":
                continue
            tmp, _ = heinrich_test(state, action_list[ix], False, iterations=20)
            # print("Best dir: " + str(math.degrees(tmp)) + " for action: " + action_list[idx].name)
            if np.abs(tmp) < np.abs(best_dir):
                best_dir = tmp
                best_action = ix
        # print("Best dir: " + str(math.degrees(best_dir)) + " for action: " + action_list[best_action].name)

        # Rotate the robot so that the shooting angle == best_dir
        state.pose.rotation = state.pose.rotation + best_dir

        new_action = a.Action("new_action", action_list[best_action].speed, action_list[best_action].speed_std,
                              action_list[best_action].angle, 0)
        # after turning evaluate the best action again to calculate the expected ball position
        actions_consequences = []
        single_consequence = a.ActionResults([])
        actions_consequences.append(Sim.simulate_consequences(new_action, single_consequence, state, num_particles=30))

        # expected_ball_pos should be in local coordinates for rotation calculations
        expected_ball_pos = actions_consequences[0].expected_ball_pos_mean

        # calculate the time needed
        rotation = np.arctan2(expected_ball_pos.y, expected_ball_pos.x)

        # update the robots position
        state.update_pos(state.pose * expected_ball_pos, math.degrees(state.pose.rotation + rotation))

        sim_data.append(copy.deepcopy(state))
    return sim_data


def main():
    plt.clf()
    axes = plt.gca()
    tools.draw_field(axes)

    ball_line_current = simulate_goal_cycle_current_impl(iterations=7)
    ball_line_particle = simulate_goal_cycle_particle(iterations=4, use_sidekicks=True)  # mixed scheme
    ball_line_particle2 = simulate_goal_cycle_particle(iterations=3, use_sidekicks=False)

    draw_robot_walk_lines(axes, ball_line_current, position_color='blue')
    draw_robot_walk_lines(axes, ball_line_particle, position_color='red')  # mixed scheme
    draw_robot_walk_lines(axes, ball_line_particle2, position_color='yellow')

    plt.show()


if __name__ == "__main__":
    main()
