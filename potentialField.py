import numpy as np


# works 2d?
def gaussian(x, y, mu_x, mu_y, sigma_x, sigma_y):
  fac_x = np.power(x - mu_x, 2.0) / (2.0 * sigma_x * sigma_x)
  fac_y = np.power(y - mu_y, 2.0) / (2.0 * sigma_y * sigma_y)
  return np.exp(-1.0 * (fac_x + fac_y))


def slope(x, y, slope_x, slope_y):
  return slope_x * x + slope_y * y


def evaluateAction(x, y):
  # evaluates the potentialfiel at a x,y position
  x_length = 6000
  y_length = 4000

  xPosOpponentGroundline = x_length / 2

  xPosOpponentGoal = xPosOpponentGroundline
  xPosOwnGoal = -xPosOpponentGoal

  yPosLeftSideline = y_length / 2

  sigmaX = xPosOpponentGoal / 2.0
  sigmaY = yPosLeftSideline / 2.5
  f = slope(x, y, -1.0 / xPosOpponentGoal, 0.0) - gaussian(x, y, xPosOpponentGoal, 0.0, sigmaX, sigmaY) + gaussian(x, y,
                                                                                                                   xPosOwnGoal,
                                                                                                                   0.0,
                                                                                                                   1.5 * sigmaX,
                                                                                                                   sigmaY)
  return f
