from math2d import * 
import numpy as np
from matplotlib import pyplot as plt
import random
from sklearn.neighbors import NearestNeighbors
from hyperopt import hp, fmin, tpe, Trials
from scipy.optimize import minimize

class Optimizer:
  def __init__(self, gt, mes):
    self.gt = gt
    self.mes = mes
    self.knn = NearestNeighbors(n_neighbors=1).fit([[x.x, x.y] for x  in self.gt])
  def optimize(self):
#    space = [hp.uniform("x", -5.0, 5.0), hp.uniform("y", -5, 5), hp.uniform("theta", -math.pi, math.pi)]
#    trials = Trials()
#    best = fmin(self.objective, space, algo=tpe.suggest, max_evals=100, trials=trials)
#    p = Pose2D()
#    p.translation = Vector2(best["x"], best["y"])
#    p.rotation = best["theta"]
    res = minimize(self.objective, [0.0, 0.0, 0.0])
    best = res.x 
    p = Pose2D()
    p.translation = Vector2(best[0], best[1])
    p.rotation = best[2]
    return p
  def objective(self, args):
    p = Pose2D()
    p.translation = Vector2(args[0], args[1])
    p.rotation = args[2]
    guess = [p * Vector2(x.x, x.y) for x in self.mes]
    distances = [self.knn.kneighbors([x.x, x.y])[0][0][0] for x in guess]
    metric = np.sum(np.square(distances))
    return metric

if __name__ == "__main__":

  gt = [Vector2(0.0, 0.0)]
  for i  in range(10):
    gt.append(gt[-1] + Vector2(0.1, 0.0))
  for i  in range(20):
    gt.append(gt[-1] + Vector2(0.0, 0.1))
  for i  in range(10):
    gt.append(gt[-1] + Vector2(-0.1, 0.0))
  for i  in range(20):
    gt.append(gt[-1] + Vector2(0.0, -0.1))
  plt.plot([x.x for x in gt], [x.y for x in gt])

  diff = Pose2D()
  diff.translation = Vector2(-1.0, 2.0)
  diff.rotation = math.pi/4.0

  mes = [diff*x for x in gt[10:40]]
  noise = 0.05
  mes = [Vector2(x.x + random.gauss(0.0, noise), x.y + random.gauss(0.0, noise)) for x in mes]
  plt.plot([x.x for x in mes], [x.y for x in mes])

  solution = []
  for i in range(5):
    opt = Optimizer(gt, mes)
    antidiff = opt.optimize()

    solution = [antidiff * x for x in mes]
    mes = solution
  plt.plot([x.x for x in solution], [x.y for x in solution], "r")
  
  plt.gca().set_aspect("equal", "datalim")
  plt.show()
