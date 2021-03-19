from __future__ import division
import numpy as np
from tools import field_info as field

""" General Functions """


def gaussian(x, y, mu_x, mu_y, sigma_x, sigma_y):
    fac_x = np.power(x - mu_x, 2.0) / (2.0 * sigma_x * sigma_x)
    fac_y = np.power(y - mu_y, 2.0) / (2.0 * sigma_y * sigma_y)
    return np.exp(-1.0 * (fac_x + fac_y))


def slope(x, y, slope_x, slope_y):
    return slope_x * x + slope_y * y


""" Current Potentialfield"""


def evaluate_action(results, state):
    sum_potential = 0.0
    number_of_actions = 0.0
    for p in results.positions():
        if p.cat() == "INFIELD" or p.cat() == "OPPGOAL":
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
    sum_potential = 0.0
    number_of_actions = 0.0
    for p in results.positions():
        if p.cat() == "INFIELD" or p.cat() == "OPPGOAL":
            sum_potential += evaluate_single_pos_with_robots(state.pose * p.pos(),
                                                             state.opp_robots, state.own_robots)
            number_of_actions += 1

    assert number_of_actions > 0
    sum_potential /= number_of_actions
    return sum_potential


def evaluate_single_pos_with_robots(ball_pos, opp_robots, own_robots):
    # gets called by evaluate_action_with_robots
    f = evaluate_single_pos(ball_pos)

    for opp_rob in opp_robots:  # adding influence of own and opponent robots into the field
        f_rob = robot_field_opp(opp_rob, ball_pos)

        # this makes the influence region uniform - it might not be what we want
        if f_rob != 0:
            if f_rob + f <= 1:
                f = f_rob
            else:
                f += f_rob

    for own_rob in own_robots:
        f_rob = robot_field_own(own_rob, ball_pos)
        if f_rob != 0:
            if f - f_rob <= -0.5:
                f -= f_rob
            else:
                f = -f_rob
    return f


def evaluate_single_w_robots_new(ball_pos, opp_robots, own_robots):
    # gets called by evaluate_action_with_robots
    field_value = evaluate_single_pos(ball_pos)

    for own_rob in own_robots:  # adding influence of own and opponent robots into the field
        rob_field = robot_field_own(own_rob, ball_pos)

        if rob_field == 0:  # robot is not relevant for action
            continue
        else:
            # continue simulation
            field_value += rob_field

    return field_value


def robot_field_own(robot_pos, ball_pos):
    # gets called by evaluate_single_pos_with_robots
    vel_rot = 60  # grad per second
    vel_walk = 200  # mm per second

    # ball_pos = robot_pos / ball_pos  # somehow the operator is not recognized here,
    # so I copied the implementation of the transformation here
    ball_pos = (ball_pos - robot_pos.translation).rotate(-robot_pos.rotation)

    # evaluation function
    angle = np.degrees(np.arctan2(ball_pos.y, ball_pos.x))
    rot_time = np.abs(angle / vel_rot)
    distance = np.hypot(ball_pos.x, ball_pos.y)
    distance_time = distance / vel_walk
    total_time = distance_time + rot_time

    if total_time >= 5:  # 5 sekunden scheint sehr viel
        total_time = 5

    total_time /= 5.
    total_time = 1 - total_time

    # multiply potential field of the Robot with a*exp(-b*x^2) (with a,b real)
    # maybe b = 4500^2 -> exp(-(x/4500)^2) bei 4500 bei 0.36 bei 3000 bei 0.3
    # Grund: Vor Toren soll nicht gepasst werden. in der Mitte am Ungefährlichsten
    # Vielleicht mittelpunkt in gegnerische Hälfte verschieben
    return total_time


def robot_field_opp(robot_pos, ball_pos):
    # gets called by evaluate_single_pos_with_robots
    vel_rot = 60  # grad pro second
    vel_walk = 200  # mm pro second

    # ball_pos = robot_pos / ball_pos  # somehow the operator is not recognized here,
    # so I copied the implementation of the transformation here
    ball_pos = (ball_pos - robot_pos.translation).rotate(-robot_pos.rotation)

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
