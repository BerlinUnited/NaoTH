from __future__ import division
import numpy as np
from tools import field_info as field


def gaussian(x, y, mu_x, mu_y, sigma_x, sigma_y):
    fac_x = np.power(x - mu_x, 2.0) / (2.0 * sigma_x * sigma_x)
    fac_y = np.power(y - mu_y, 2.0) / (2.0 * sigma_y * sigma_y)
    return np.exp(-1.0 * (fac_x + fac_y))


def slope(x, y, slope_x, slope_y):
    return slope_x * x + slope_y * y


def evaluate_action(ball_pos):  # evaluates the potential field at a x,y position
    x = ball_pos.x
    y = ball_pos.y
    sigma_x = field.x_opponent_goal / 2.0
    sigma_y = field.y_left_sideline / 2.5
    f = slope(x, y, -1.0 / field.x_opponent_goal, 0.0)\
        - gaussian(x, y, field.x_opponent_goal, 0.0, sigma_x, sigma_y)\
        + gaussian(x, y, field.x_own_goal, 0.0, 1.5 * sigma_x, sigma_y)
    return f


def evaluate_action2(results, state):
    sum_potential = 0.0
    number_of_actions = 0.0
    for p in results.positions():
        if p.cat() == "INFIELD" or p.cat() == "OPPGOAL":
            sum_potential += evaluate_action(state.pose * p.pos())
            number_of_actions += 1

    assert number_of_actions > 0
    sum_potential /= number_of_actions
    return sum_potential


# Todo write a new function like evaluate_action2 which also takes into consideration the potentialfields of the teammates
# then build a static test case which shows what happens when teammates are considered

# TODO use better potential field -> like in timeEstimation.py

def robot_field_simple(robot_pos, ball_pos):

    vel_rot = 60.
    vel_walk = 200.

    x = ball_pos.x
    y = ball_pos.y
    sigma_x = field.x_opponent_goal / 2.0
    sigma_y = field.y_left_sideline / 2.5

    # check if ball near robot

    translated_ball_pos = ball_pos - robot_pos
    distance = translated_ball_pos.abs()

    if distance > 1000.:
        return 0.0  # if ball if to far away the robot field has no impact

    # evaluation function

    value = distance / vel_walk  # time to ball, very naive evaluation
    value /= 5.
    value = 1 - value
    print(value)
    return value


def robot_field(robot_pos, ball_pos):
    # TODO check if ball_pos is in local coordinates
    vel_rot = 60  # grad pro second
    vel_walk = 200  # mm pro second

    x = ball_pos.x
    y = ball_pos.y

    # check if ball near robot -> TODO  Maybe not needed

    #translated_ball_pos = ball_pos - robot_pos
    #distance = translated_ball_pos.abs()

    #if distance > 1000.:
    #    return 0.0  # if ball if to far away the robot field has no impact

    # evaluation function
    angle = np.degrees(np.arctan2(y, x))
    rot_time = np.abs(angle / vel_rot)
    distance = np.hypot(x, y)
    distance_time = distance / vel_walk
    total_time = distance_time + rot_time

    if total_time >= 5:
        total_time = 5

    # total_time -= 5
    # TEST
    total_time /= 5.
    total_time = 1 - total_time
    return total_time


def evaluate_action_with_robots(ball_pos, opp_robots, own_robots):
    x = ball_pos.x
    y = ball_pos.y
    sigma_x = field.x_opponent_goal / 2.0
    sigma_y = field.y_left_sideline / 2.5
    f = slope(x, y, -1.0 / field.x_opponent_goal, 0.0) \
        - gaussian(x, y, field.x_opponent_goal, 0.0, sigma_x, sigma_y) \
        + gaussian(x, y, field.x_own_goal, 0.0, 1.5 * sigma_x, sigma_y)

    for opp_rob in opp_robots:  # adding influence of own and opponent robots into the field
        f_rob = robot_field(opp_rob, ball_pos)
        if f_rob != 0:
            if f_rob + f <= 1 and f_rob != 0.:
                f = f_rob
            else:
                f += f_rob

    for own_rob in own_robots:
        f_rob = robot_field(own_rob, ball_pos)
        if f_rob != 0:
            if f - f_rob <= -0.5:
                f -= f_rob
            else:
                f = -f_rob
    return f


def benji_field(results, state, opp_robots, own_robots):
    sum_potential = 0.0
    number_of_actions = 0.0
    for p in results.positions():
        if p.cat() == "INFIELD" or p.cat() == "OPPGOAL":
            sum_potential += evaluate_action_with_robots(state.pose * p.pos(), opp_robots, own_robots)
            number_of_actions += 1

    assert number_of_actions > 0
    sum_potential /= number_of_actions
    return sum_potential


def evaluate_action_with_robots2(ball_pos, opp_robots, own_robots):
    x = ball_pos.x
    y = ball_pos.y
    sigma_x = field.x_opponent_goal / 2.0
    sigma_y = field.y_left_sideline / 2.5

    f = 0
    for opp_rob in opp_robots:  # adding influence of own and opponent robots into the field
        f += robot_field_simple(opp_rob, ball_pos)
    for own_rob in own_robots:
        f -= robot_field_simple(own_rob, ball_pos)
    if f == 0:
        f = slope(x, y, -1.0 / field.x_opponent_goal, 0.0) \
            - gaussian(x, y, field.x_opponent_goal, 0.0, sigma_x, sigma_y) \
            + gaussian(x, y, field.x_own_goal, 0.0, 1.5 * sigma_x, sigma_y)
    return f
