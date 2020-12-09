from __future__ import division
import numpy as np
import field_info as field
import pickle
# from evaluation.potentialfield_generated_plot import cleanup_nan_values
from action import Category

import Simulation as Sim
import action as a
from naoth import math2d as m2d
import copy

""" General Functions """
gen_field = []
nx = []
ny = []


def gaussian(x, y, mu_x, mu_y, sigma_x, sigma_y):
    fac_x = ((x - mu_x) / sigma_x) ** 2
    fac_y = ((y - mu_y) / sigma_y) ** 2
    return np.exp(-0.5 * (fac_x + fac_y))


def slope(x, y, slope_x, slope_y):
    return slope_x * x + slope_y * y


""" Current Potentialfield"""


def evaluate_action(results, state):
    sum_potential = 0.0
    number_of_actions = 0.0
    for p in results.positions():
        if p.cat() == Category.INFIELD or p.cat() == Category.OPPGOAL:
            sum_potential += evaluate_single_pos(state.pose * p.pos())
            number_of_actions += 1

    assert number_of_actions > 0
    sum_potential /= number_of_actions
    return sum_potential


def evaluate_single_pos(ball_pos):  # evaluates the potential field at a x,y position
    # gets called by evaluate_action2
    sigma_x = field.x_opponent_goal / 2.0
    sigma_y = field.y_left_sideline / 2.5
    f = slope(ball_pos.x, ball_pos.y, -1.0 / field.x_opponent_goal, 0.0) \
        - gaussian(ball_pos.x, ball_pos.y, field.x_opponent_goal, 0.0, sigma_x, sigma_y) \
        + gaussian(ball_pos.x, ball_pos.y, field.x_own_goal, 0.0, 1.5 * sigma_x, sigma_y)

    return f


""" Potentialfield with other robots """


def evaluate_action_with_robots(results, state):
    pf_normal_value = evaluate_action(results, state)

    sum_potential = 0.0
    number_of_actions = 0.0
    for p in results.positions():
        if p.cat() == Category.INFIELD or p.cat() == Category.OPPGOAL:
            sum_potential += evaluate_single_pos_with_robots(state.pose * p.pos(),
                                                             state.opp_robots, state.own_robots)
            number_of_actions += 1

    assert number_of_actions > 0
    sum_potential /= number_of_actions

    squared_difference = np.abs(
        pf_normal_value - sum_potential)  # decide whether considering other robots is appropriate
    if squared_difference < 0.05:  # upper bound - how to choose?
        return pf_normal_value

    # ab hier experimental
    # new Ball pos with passing

    new_ball_pos = results.expected_ball_pos_mean

    # update state
    new_state = copy.copy(state)
    new_state.ball_position = m2d.Vector2(0.0, 0.0)  # Ball = Robot
    new_state.translation = new_ball_pos
    # new_state.rotation = pass # maybe rotation as needed for direkt / shortest path to the ball
    new_state.potential_field_function = "normal"

    # Simulation as in Simulation.py (decide_smart)

    actions_consequences = []
    # Simulate Consequences
    for action in new_state.actions:  # new_state.actions includes the possible kicks
        single_consequence = a.ActionResults([])
        actions_consequences.append(
            Sim.simulate_consequences(action, single_consequence, new_state, 30))

    best_action_with_team = Sim.decide_smart(actions_consequences, state)

    # compare new best action
    # TODO: Add simulation of a second kick without other robots to have better comparison
    # needed: action which would have been done without other robots
    # what should get returned?? to compare with other actions
    """
    if best_value > pf_normal_value:
        return sum_potential
    """

    return sum_potential


def evaluate_single_pos_with_robots(ball_pos, opp_robots, own_robots):
    # gets called by evaluate_action_with_robots
    sigma_x = field.x_opponent_goal / 2.0
    sigma_y = field.y_left_sideline / 2.5
    f = slope(ball_pos.x, ball_pos.y, -1.0 / field.x_opponent_goal, 0.0) \
        - gaussian(ball_pos.x, ball_pos.y, field.x_opponent_goal, 0.0, sigma_x, sigma_y) \
        + gaussian(ball_pos.x, ball_pos.y, field.x_own_goal, 0.0, 1.5 * sigma_x, sigma_y)

    for opp_rob in opp_robots:  # adding influence of own and opponent robots into the field
        f += robot_field_opp(opp_rob, ball_pos)

        """
        # this makes the influence region uniform - it might not be what we want
        f_rob = robot_field_opp(opp_rob, ball_pos)


        if f_rob != 0:
            if f_rob + f <= 1:
                f = f_rob
            else:
                f += f_rob
        """

    for own_rob in own_robots:
        f -= robot_field_own(own_rob, ball_pos)

        """
        f_rob = robot_field_own(own_rob, ball_pos)

        if f_rob != 0:
            if f - f_rob <= -0.5:
                f -= f_rob
            else:
                f = -f_rob
        """
    return f


def robot_field_own(robot_pos, ball_pos):
    # gets called by evaluate_single_pos_with_robots
    vel_rot = 60  # grad pro second
    vel_walk = 200  # mm pro second

    # ball_pos = robot_pos / ball_pos  # somehow the operator is not recognized here,
    # so I copied the implementation of the transformation here
    # ball_pos = (ball_pos - robot_pos.translation).rotate(-robot_pos.rotation)
    ball_pos = (ball_pos - robot_pos)

    # evaluation function
    angle = np.degrees(np.arctan2(ball_pos.y, ball_pos.x))
    rot_time = np.abs(angle / vel_rot)
    distance = np.hypot(ball_pos.x, ball_pos.y)
    distance_time = distance / vel_walk
    total_time = distance_time + rot_time

    if total_time >= 5:
        total_time = 5

    total_time /= 5.
    total_time = 1 - total_time
    return total_time


def robot_field_opp(robot_pos, ball_pos):
    # gets called by evaluate_single_pos_with_robots
    vel_rot = 60  # grad pro second
    vel_walk = 200  # mm pro second

    # ball_pos = robot_pos / ball_pos  # somehow the operator is not recognized here,
    # so I copied the implementation of the transformation here
    # ball_pos = (ball_pos - robot_pos.translation).rotate(-robot_pos.rotation)
    ball_pos = (ball_pos - robot_pos)

    # evaluation function
    angle = np.degrees(np.arctan2(ball_pos.y, ball_pos.x))
    rot_time = np.abs(angle / vel_rot)
    distance = np.hypot(ball_pos.x, ball_pos.y)
    distance_time = distance / vel_walk
    total_time = distance_time + rot_time

    if total_time >= 5:
        total_time = 5

    total_time /= 5.
    total_time = 1 - total_time
    return total_time


""" Generated Potentialfield """

"""
def evaluate_action_gen_field(results, state):
    sum_potential = 0.0
    number_of_actions = 0.0
    for p in results.positions():
        if p.cat() == Category.INFIELD or p.cat() == Category.OPPGOAL:
            sum_potential += evaluate_single_pos(state.pose * p.pos())
            number_of_actions += 1

    assert number_of_actions > 0
    sum_potential /= number_of_actions
    return sum_potential


def evaluate_single_pos_gen_field(ball_pos):
    # TODO round ball_pos.x and ball_pos.y to the nearest position int the lookup table
    ball_pos.x = int(round(x / 200.0) * 200.0)
    ball_pos.y = int(round(x / 200.0) * 200.0)
    f = gen_field[ny[ball_pos.y], nx[ball_pos.x]]
    return f
"""

if __name__ == "__main__":
    """
    data_prefix = "D:/RoboCup/Paper-Repos/Bachelor-Schlotter/data/"
    gen_field = pickle.load(open(str(data_prefix) + "potential_field_generation/potential_field_gen_own1.pickle", "rb"))

    gen_field = cleanup_nan_values(gen_field)
    # create a structure for the scalar field
    nx = {}
    ny = {}
    for pos in gen_field:
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
    for position in gen_field:
        x, y, time, _ = position
        f[ny[y], nx[x]] = time
        g[ny[y], nx[-x]] = time

    gen_field = f-g
    """
