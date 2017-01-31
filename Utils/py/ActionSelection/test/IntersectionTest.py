import math2d as m2d
from matplotlib import pyplot as plt

plt.clf()
ax = plt.gca()
# Test intersection implementation

point1 = m2d.Vector2(4400, -700)
point2 = m2d.Vector2(4400, 700)

point3 = m2d.Vector2(0, 0)
point4 = m2d.Vector2(1000, 0)

lineSegment1 = m2d.LineSegment(point1, point2)
lineSegment2 = m2d.LineSegment(point3, point4)

ax.plot([point1.x, point2.x], [point1.y, point2.y], 'white')  # lineSegment1
ax.plot([point3.x, point4.x], [point3.y, point4.y], 'white')  # lineSegment2

print lineSegment1.intersection(lineSegment2)


ax.set_xlim([-5200, 5200])  # Todo use FieldInfo Stuff
ax.set_ylim([-3700, 3700])
ax.set_axis_bgcolor('green')
ax.set_aspect("equal")
plt.show()
