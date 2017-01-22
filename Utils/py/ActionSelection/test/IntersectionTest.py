import math2d as m2d

# Test intersection implementation

point1 = m2d.Vector2(0, 0)
point2 = m2d.Vector2(6, 6)
point3 = m2d.Vector2(-4, 6)
point4 = m2d.Vector2(10, 6)
lineSegment1 = m2d.LineSegment(point1, point2)
lineSegment2 = m2d.LineSegment(point3, point4)

lineSegment1.intersection(lineSegment2)
intersectionPointOnLine = lineSegment1.intersection(lineSegment2)
print intersectionPointOnLine
print lineSegment1.point(intersectionPointOnLine)
