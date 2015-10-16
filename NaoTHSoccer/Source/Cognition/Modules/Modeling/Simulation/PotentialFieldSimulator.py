from matplotlib import pyplot as plt
import numpy as np
import math

# works 2d?
def gaussian(X, Y, muX, muY, sigmaX, sigmaY):
  facX = np.power(X-muX, 2.0) / (2.0 * sigmaX * sigmaX)
  facY = np.power(Y-muY, 2.0) / (2.0 * sigmaY * sigmaY)
  return np.exp(-1.0 * (facX + facY))

def slope(X, Y, slopeX, slopeY):
  return slopeX * X + slopeY * Y

if __name__ == "__main__":
  xLength = 6000
  yLength = 4000
  goalWidth = 1400
  goalHeight = 800

  xPosOpponentGroundline = xLength / 2
  xPosOwnGroundline = -xPosOpponentGroundline

  xPosOpponentGoal = xPosOpponentGroundline
  xPosOwnGoal = -xPosOpponentGoal

  yPosLeftSideline = yLength / 2 
  yPosRightSideline = -yPosLeftSideline

  xRange = np.linspace(xPosOwnGroundline, xPosOpponentGroundline, 1000)
  yRange = np.linspace(yPosRightSideline, yPosLeftSideline, 1000)

  X, Y = np.meshgrid(xRange, yRange)

  X = X.flatten()
  Y = Y.flatten()

  sigmaX = xPosOpponentGoal/2.0
  sigmaY = yPosLeftSideline/2.5

  f = slope(X, Y, -1.0/xPosOpponentGoal, 0.0) - gaussian(X, Y, xPosOpponentGoal, 0.0, sigmaX, sigmaY) + gaussian(X, Y, xPosOwnGoal, 0.0, 1.5*sigmaX, sigmaY) 

  f = np.reshape(f, (len(xRange), len(yRange)))

  plt.plot([0.0, 0.0], [yPosLeftSideline, yPosRightSideline], "k:")
  plt.plot([xPosOwnGroundline, xPosOpponentGroundline], [0.0, 0.0], "k:")
  CS = plt.contour(xRange, yRange, f, 20, cmap="cool_r")
  plt.clabel(CS, inline=1, fontsize=10)
  plt.gca().set_aspect("equal")
  plt.show()
