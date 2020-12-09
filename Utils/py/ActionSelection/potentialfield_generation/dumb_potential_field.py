from __future__ import division

import numpy as np
from matplotlib import pyplot as plt
from naoth.math import *

from tools import field_info as field
from tools import tools

# create a structure for the scalar field
x_step = 200
y_step = 200
nx = {}
ny = {}
for x in range(int(-field.x_length * 0.5), int(field.x_length * 0.5) + x_step, x_step):
    nx[x] = x
for y in range(int(-field.y_length * 0.5), int(field.y_length * 0.5) + y_step, y_step):
    ny[y] = y

nxi = np.array(sorted(nx.keys()))
nyi = np.array(sorted(ny.keys()))

for i, v in enumerate(nxi):
    nx[v] = i

for i, v in enumerate(nyi):
    ny[v] = i

f = np.zeros((len(ny), len(nx)))
g = np.zeros((len(ny), len(nx)))

# create opponent goal line as line segment
left_goal_post = Vector2(field.x_opponent_groundline, field.y_left_goalpost)
right_goal_post = Vector2(field.x_opponent_groundline, field.y_right_goalpost)
goal_line = LineSegment(left_goal_post, right_goal_post)

plt.clf()
tools.draw_field(plt.gca())
ax = plt.gca()

# create the scalar fields
for x in range(int(-field.x_length * 0.5), int(field.x_length * 0.5), x_step):
    for y in range(int(-field.y_length * 0.5), int(field.y_length * 0.5), y_step):
        a = np.array([x + x_step / 2, y + y_step / 2])
        # b = np.array([4500, 0])
        projection = goal_line.projection(Vector2(x, y))
        b = np.array([projection.x, projection.y])
        f[ny[y], nx[x]] = np.sqrt(np.sum((a - b) ** 2))

# Plot potentials
plt.pcolor(nxi, nyi, f - g, cmap="Greys_r", alpha=0.8, zorder=100)
plt.show()

"""
ax.add_patch(
          patches.Rectangle(
          (x-x_step/2, y-y_step/2), x_step, y_step,
            facecolor=str(np.sqrt(np.sum((a-b)**2))/10000)
          )
        )
"""
