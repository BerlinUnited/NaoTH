import numpy as np
import field_info as field

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
