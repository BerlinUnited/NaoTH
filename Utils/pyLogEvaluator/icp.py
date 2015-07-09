from math2d import * 
import numpy as np
from matplotlib import pyplot as plt
import random
from sklearn.neighbors import NearestNeighbors
from scipy.optimize import minimize

class Optimizer:
  def __init__(self, gt, mes):
    self.n_select = min(10, len(mes))
    self.gt = gt
    self.mes = mes
    self.knn = NearestNeighbors(n_neighbors=1).fit([[x.x, x.y] for x  in self.gt])
    self.nbrs = []
    self.selected = []
  def optimize(self):
    self.selected = np.random.choice(self.mes, self.n_select)
    self.nbrs = [self.gt[self.knn.kneighbors([x.x, x.y])[1][0][0]] for x in self.selected]
    res = minimize(self.objective, [0.0, 0.0, 0.0])
    best = res.x 
    p = Pose2D()
    p.translation = Vector2(best[0], best[1])
    p.rotation = best[2]
    return p, res.fun
  def objective(self, args):
    p = Pose2D()
    p.translation = Vector2(args[0], args[1])
    p.rotation = args[2]
    guess = [p * Vector2(x.x, x.y) for x in self.selected]
    diffs = [x1-x2 for x1,x2 in zip(guess, self.nbrs)]
    distances = np.hypot([x.x for x in diffs], [x.y for x in diffs])
    metric = np.sum(np.square(distances))
    return metric

if __name__ == "__main__":

  dx = 100.0
  xLength = 9000;
  yLength = 6000;
  gt = []
  gt.extend([Vector2(x, 0.0) for x in np.arange(0.0, xLength, dx)])
  gt.extend([Vector2(x, yLength) for x in np.arange(0.0, xLength, dx)])
  gt.extend([Vector2(0.0, y) for y in np.arange(0.0, yLength, dx)])
  gt.extend([Vector2(xLength, y) for y in np.arange(0.0, yLength, dx)])
  xPenaltyAreaLength = 600;
  yPenaltyAreaLength = 2200;
  gt.extend([Vector2(x, yLength/2.0-yPenaltyAreaLength/2.0) for x in np.arange(0.0, xPenaltyAreaLength, dx)])
  gt.extend([Vector2(x, yLength/2.0+yPenaltyAreaLength/2.0) for x in np.arange(0.0, xPenaltyAreaLength, dx)])
  gt.extend([Vector2(xPenaltyAreaLength, y) for y in np.arange(yLength/2.0-yPenaltyAreaLength/2.0, yLength/2.0+yPenaltyAreaLength/2.0, dx)])
  gt.extend([Vector2(xLength-x, yLength/2.0-yPenaltyAreaLength/2.0) for x in np.arange(0.0, xPenaltyAreaLength, dx)])
  gt.extend([Vector2(xLength-x, yLength/2.0+yPenaltyAreaLength/2.0) for x in np.arange(0.0, xPenaltyAreaLength, dx)])
  gt.extend([Vector2(xLength - xPenaltyAreaLength, y) for y in np.arange(yLength/2.0-yPenaltyAreaLength/2.0, yLength/2.0+yPenaltyAreaLength/2.0, dx)])
  gt.extend([Vector2(xLength/2, y) for y in np.arange(0.0, yLength, dx)])
  centerCircleRadius = 750;
  gt.extend([Vector2(xLength/2+math.cos(a)*centerCircleRadius, yLength/2+math.sin(a)*centerCircleRadius) for a in np.linspace(0, math.pi*2, 2*math.pi*centerCircleRadius/dx)])

  diff = Pose2D()
  diff.translation = Vector2(-1.0, 2.0)
  diff.rotation = math.pi/4.0

  mes = []
  while(len(mes) < 20):
    i = random.randint(0, len(gt)-1)
    if gt[i].x > -1000 and gt[i].x < 3000 and gt[i].y > 1000 and gt[i].y < 3000:
      mes.append(diff*gt[i])

  noise = 0
  mes = [Vector2(x.x + random.gauss(0.0, noise), x.y + random.gauss(0.0, noise)) for x in mes]

  plt.ion()

  solution = [Vector2(x.x, x.y) for x in mes]
  for i in range(25):
    opt = Optimizer(gt, solution)
    antidiff, fun = opt.optimize()
    print fun

    solution = [antidiff * x for x in solution]
    plt.clf()
    plt.plot([x.x for x in gt], [x.y for x in gt], "k.")
    plt.plot([x.x for x in mes], [x.y for x in mes], "b.")
    plt.plot([x.x for x in solution], [x.y for x in solution], "r.")
    plt.gca().set_aspect("equal", "datalim")
    plt.draw()
