#!/usr/bin/python

from util import *
import features as feat
from matplotlib import pyplot as plt

X, labels = load_data(sys.argv[1])

idxBall = labels[:] == 1
idxNoBall = labels[:] == 0

X_ball = X[idxBall]
X_noball = X[idxNoBall]

f, p = plt.subplots(2, 10)

for i in range(0, 10):
    p[0, i].plot(feat.histo(X_ball[i].reshape((12, 12))).T)
    p[1, i].plot(feat.histo(X_noball[i].reshape((12, 12))).T)

plt.show()
