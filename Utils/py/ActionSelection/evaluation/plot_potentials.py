from __future__ import division
from matplotlib import pyplot as plt
from matplotlib.mlab import griddata
import numpy as np
import pickle

# TODO use data that the matlab script uses as well
# one file has all the potentials (time to goal) for one team
potentials_own = pickle.load(open("../data/potential_field_generation/potential_field_gen_own1.pickle", "rb"))
# potentials_opp = pickle.load(open("../data/potential_field_generation/potential_field_gen_opp0.pickle", "rb"))

x_own = np.array([])
y_own = np.array([])
times_own = np.array([])

i = 0

for own in potentials_own:
    i += 1
    x, y, time, angle = own
    x_own = np.append(x_own, x)  # x_own.append(x)
    y_own = np.append(y_own, y)  # y_own.append(y)
    times_own = np.append(times_own, time)  # times_own.append(time)

# Through the unstructured data get the structured data by interpolation
xi = np.linspace(x_own.min()-1, x_own.max()+1, 100)
yi = np.linspace(y_own.min()-1, y_own.max()+1, 100)
zi = griddata(x_own, y_own, times_own, xi, yi, interp='linear')

CS = plt.contourf(xi, yi, zi, 10, alpha=0.5, cmap="coolwarm", frameon=False)
plt.colorbar()
plt.show()
