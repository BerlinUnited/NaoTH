from math2d import * 
import numpy as np
from matplotlib import pyplot as plt
import random
from sklearn.neighbors import NearestNeighbors
from hyperopt import hp, fmin, tpe, Trials
from scipy.optimize import minimize

class Optimizer:
  def __init__(self, gt):
    self.n_select = 1
    self.gt = gt
    self.mes = []
    self.knn = NearestNeighbors(n_neighbors=1).fit([[x.x, x.y] for x  in self.gt])
    self.nbrs = []
    self.selected = []
  def optimize(self, mes, method="BFGS"):
    self.mes = mes
    self.n_select = random.randint(int(0.2*len(mes)), int(0.8*(len(mes)-1)))
    self.selected = np.random.choice(self.mes, self.n_select)
    self.nbrs = [self.gt[self.knn.kneighbors([x.x, x.y])[1][0][0]] for x in self.selected]
    if method == "hyperopt":
      space = [hp.uniform("x", -5.0, 5.0), hp.uniform("y", -5, 5), hp.uniform("theta", -math.pi, math.pi)]
      trials = Trials()
      best = fmin(self.objective, space, algo=tpe.suggest, max_evals=100, trials=trials)
      p = Pose2D()
      p.translation = Vector2(best["x"], best["y"])
      p.rotation = best["theta"]
      fun = trials.average_best_error()
      return p, fun/self.n_select
    elif method == "BFGS":
      res = minimize(self.objective, [0.0, 0.0, 0.0])
      best = res.x 
      p = Pose2D()
      p.translation = Vector2(best[0], best[1])
      p.rotation = best[2]
      return p, res.fun/self.n_select
    else:
      return
  def objective(self, args):
    p = Pose2D()
    p.translation = Vector2(args[0], args[1])
    p.rotation = args[2]
    guess = [p * Vector2(x.x, x.y) for x in self.selected]
    diffs = [x1-x2 for x1,x2 in zip(guess, self.nbrs)]
    distances = np.hypot([x.x for x in diffs], [x.y for x in diffs])
    # throw away worst 10%
    distances = np.sort(distances)
    metric = np.sum(np.square(distances[:len(distances)-int(0.1*len(distances))]))
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
  diff.translation = Vector2(random.uniform(-1000, 1000), random.uniform(-1000, 1000))
  diff.rotation = random.uniform(-math.pi/4, math.pi/4)

  mes = []
  while(len(mes) < 100):
    i = random.randint(0, len(gt)-1)
    if gt[i].x > -1000 and gt[i].x < 5000 and gt[i].y > -1000 and gt[i].y < 3000:
      mes.append(diff*gt[i])

  noise = 10
  mes = [Vector2(x.x + random.gauss(0.0, noise), x.y + random.gauss(0.0, noise)) for x in mes]

  plt.ion()

  solution = [Vector2(x.x, x.y) for x in mes]
  opt = Optimizer(gt)
  for i in range(40):
    antidiff = Pose2D()
    antidiff, fun = opt.optimize(solution, "BFGS")
    print fun

    solution = [antidiff * x for x in solution]
    plt.clf()
    plt.plot([x.x for x in gt], [x.y for x in gt], "k.")
    plt.plot([x.x for x in mes], [x.y for x in mes], "b.")
    plt.plot([x.x for x in solution], [x.y for x in solution], "r.")
    plt.gca().set_aspect("equal", "datalim")
    plt.draw()
