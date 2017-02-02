import numpy as np
import field_info as field


def gaussian(x, y, mu_x, mu_y, sigma_x, sigma_y):
    fac_x = np.power(x - mu_x, 2.0) / (2.0 * sigma_x * sigma_x)
    fac_y = np.power(y - mu_y, 2.0) / (2.0 * sigma_y * sigma_y)
    return np.exp(-1.0 * (fac_x + fac_y))


def slope(x, y, slope_x, slope_y):
    return slope_x * x + slope_y * y


def evaluate_action(x, y):  # evaluates the potential field at a x,y position

    sigma_x = field.x_opponent_goal / 2.0
    sigma_y = field.y_left_sideline / 2.5
    f = slope(x, y, -1.0 / field.x_opponent_goal, 0.0)\
        - gaussian(x, y, field.x_opponent_goal, 0.0, sigma_x, sigma_y)\
        + gaussian(x, y, field.x_own_goal, 0.0, 1.5 * sigma_x, sigma_y)
    return f
