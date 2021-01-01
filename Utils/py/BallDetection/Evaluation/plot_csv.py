import matplotlib.pyplot as plt
import sys
import numpy as np

scores = np.genfromtxt(sys.argv[1], usecols=(1), skip_header=1, delimiter=",")

scores = np.sort(scores)

plt.style.use('seaborn')
plt.plot(scores)
plt.show()