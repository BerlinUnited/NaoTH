from math2d import * 
import numpy as np
from matplotlib import pyplot as plt
import random
from sklearn.neighbors import NearestNeighbors
from hyperopt import hp, fmin, tpe, Trials
from scipy.optimize import minimize
import cPickle


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
    self.n_select = int(0.2*len(mes))
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
      random_guesses = [[random.gauss(0.0, 1000), random.gauss(0.0, 1000), random.uniform(-math.pi, math.pi)] for i in range(100)]
      random_guesses.append([0.0, 0.0, 0.0])
      metrics = [[self.objective(guess), guess] for guess in random_guesses]
      metrics.sort(key=lambda x:x[0])
      res = minimize(self.objective, metrics[0][1])
      best = res.x
      fun = res.fun
      p = Pose2D()
      p.translation = Vector2(best[0], best[1])
      p.rotation = best[2]
      return p, fun/self.n_select
    else:
      return
  def objective(self, args):
    p = Pose2D()
    p.translation = Vector2(args[0], args[1])
    p.rotation = args[2]
    guess = [p * Vector2(x.x, x.y) for x in self.selected]
    diffs = [x1-x2 for x1,x2 in zip(guess, self.nbrs)]
    distances = np.hypot([x.x for x in diffs], [x.y for x in diffs])
    # throw away worst 20%
    distances = np.sort(distances)
    metric = np.sum(np.square(distances[:len(distances)-int(0.2*len(distances))]))
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

  data = cPickle.load(open("lines.pick", "rb"))

  for mes in data[::100]:
    mes_pruned = [p for p in mes if math.hypot(p.x, p.y) < 15000]
    plt.ion()
    solution = [Vector2(x.x, x.y) for x in mes_pruned]
    history = []
    opt = Optimizer(gt)
    for i in range(10):
      antidiff = Pose2D()
      antidiff, fun = opt.optimize(solution, "BFGS")
      history.append(fun)
      print fun
      if fun < 100:
        break

      solution = [antidiff * x for x in solution]
      plt.clf()
#      plt.subplot(211)
      plt.plot([x.x for x in gt], [x.y for x in gt], "k.")
      plt.plot([x.x for x in mes_pruned], [x.y for x in mes_pruned], "b.")
      plt.plot([x.x for x in solution], [x.y for x in solution], "r.")
      plt.gca().set_aspect("equal", "datalim")
#      plt.subplot(212)
#      plt.plot(history)
#      plt.gca().set_yscale("log")
#      plt.grid()
      plt.draw()
