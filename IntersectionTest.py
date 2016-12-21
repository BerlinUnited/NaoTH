import math2d as m2d

'''
#Bad: checks intersection of lines not linesegments
def line_intersection(line1, line2):
    xdiff = (line1[0].x - line1[1].x, line2[0].x - line2[1].x)
    ydiff = (line1[0].y - line1[1].y, line2[0].y - line2[1].y)

    #xdiff = (line1[0][0] - line1[1][0], line2[0][0] - line2[1][0])
    #ydiff = (line1[0][1] - line1[1][1], line2[0][1] - line2[1][1])

    def det(a, b):
        return a[0] * b[1] - a[1] * b[0]

    def detv2(a,b):#takes vectors
      return a.x * b.y - a.y * b.x

    div = det(xdiff, ydiff)
    if div == 0:
       raise Exception('lines do not intersect')

    d = (detv2(line1[0],line1[1]), detv2(line2[0],line2[1]))
    x = det(d, xdiff) / div
    y = det(d, ydiff) / div
    return x, y

test1 = m2d.Vector2(0,0)
test2 = m2d.Vector2(10,10)
test3 = m2d.Vector2(-10,10)
test4 = m2d.Vector2(20,23)

#line_intersection(((0,0),(10,10)), ((-10,10),(10,-10)))
print line_intersection((test1,test2), (test3,test4))
'''

#Also bad: since parallel line make some problems
def ccw(A,B,C):
    return (C.y-A.y)*(B.x-A.x) > (B.y-A.y)*(C.x-A.x)

def intersect(A,B,C,D):
        return ccw(A,C,D) != ccw(B,C,D) and ccw(A,B,C) != ccw(A,B,D)

test1 = m2d.Vector2(0,0)
test2 = m2d.Vector2(10,10)
test3 = m2d.Vector2(-10,10)
test4 = m2d.Vector2(10,-10)

print intersect(test1,test2,test3,test4)
