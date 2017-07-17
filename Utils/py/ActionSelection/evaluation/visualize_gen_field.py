from __future__ import division
from matplotlib import pyplot as plt
import pickle
import numpy as np

from tools import tools
"""
 should read in pickle files from data/potential_field_generation
 Pickle File format:  x, y, time, angle
"""

# gen_field = pickle.load(open("../data/potential_field_generation/potential_field_gen_own0.pickle", "rb"))
gen_field_own = pickle.load(open("../data/potential_field_generation/potential_field_gen_own23.pickle", "rb"))

plt.clf()
tools.draw_field()
axes = plt.gca()
patches = []

nx = {}
ny = {}
for pos in gen_field_own:
    x, y, time, angle = pos
    nx[x] = x
    ny[y] = y


nxi = np.array(sorted(nx.keys()))
nyi = np.array(sorted(ny.keys()))

for i, v in enumerate(nxi):
    nx[v] = i

for i, v in enumerate(nyi):
    ny[v] = i

f = np.zeros((len(ny), len(nx)))
g = np.zeros((len(ny), len(nx)))
for pos in gen_field_own:
    x, y, time, _ = pos

    f[ny[y], nx[x]] = time
    g[ny[y], nx[-x]] = time


plt.pcolor(nxi, nyi, f-g, cmap="Greys_r", alpha=0.5)
plt.show()
