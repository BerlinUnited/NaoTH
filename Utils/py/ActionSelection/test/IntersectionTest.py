import math2d as m2d

def line_intersection(line1, line2):
    xdiff = (line1[0][0] - line1[1][0], line2[0][0] - line2[1][0])
    ydiff = (line1[0][1] - line1[1][1], line2[0][1] - line2[1][1])

    def det(a, b):
        return a[0] * b[1] - a[1] * b[0]

    div = det(xdiff, ydiff)
    if div == 0:
       raise Exception('lines do not intersect')

    d = (det(line1[0],line1[1]), det(line2[0],line2[1]))
    x = det(d, xdiff) / div
    y = det(d, ydiff) / div
    return x, y


def ccw(A,B,C):
    return (C.y-A.y)*(B.x-A.x) > (B.y-A.y)*(C.x-A.x)

def intersect(A,B,C,D):
        return ccw(A,C,D) != ccw(B,C,D) and ccw(A,B,C) != ccw(A,B,D)


test1 = m2d.Vector2(0,0)
test2 = m2d.Vector2(10,10)
test3 = m2d.Vector2(-10,10)
test4 = m2d.Vector2(10,-10)

lineTest1 = m2d.LineSegment(test1.x,test1.y,test2.x,test2.y)
lineTest2 = m2d.LineSegment(test3.x,test3.y,test4.x,test4.y)

print line_intersection(lineTest1, lineTest2)
print intersect(test1,test2,test3,test4)
