from matplotlib import pyplot as plt
from matplotlib.patches import Circle
import math
import math2d as m2d

def line_intersection(line1, line2):#Todo implement this algorithm for type linesegments
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

def line_intersection2(line1, line2):#Todo implement this algorithm for type linesegments
    xdiff = (line1.point1X - line1.point2X, line2.point1X - line2.point2X)
    ydiff = (line1.point1Y - line1.point2Y, line2.point1Y - line2.point2Y)

    def det(a, b):
        return a[0] * b[1] - a[1] * b[0]

    div = det(xdiff, ydiff)
    if div == 0:
       raise Exception('lines do not intersect')

    d = (det(line1.point1X, line1.point2X, line1[1]), det(line2[0],line2[1]))
    x = det(d, xdiff) / div
    y = det(d, ydiff) / div
    return x, y

def ccw(A,B,C):
    return (C.y-A.y)*(B.x-A.x) > (B.y-A.y)*(C.x-A.x)

def intersect(A,B,C,D):
        return ccw(A,C,D) != ccw(B,C,D) and ccw(A,B,C) != ccw(A,B,D)

def drawField():
  ax = plt.gca()
  ax.plot([0,0],[-3000, 3000],'white') #Middleline
  ax.plot([4500,4500],[-3000, 3000],'white')
  ax.plot([-4500,-4500],[-3000, 3000],'white')
  #
  ax.plot([-4500,4500],[3000, 3000],'white')
  ax.plot([-4500,4500],[-3000, -3000],'white')

  ax.plot([3900,3900],[-1100, 1100],'white') #opppenalty
  ax.plot([4500,3900],[1100, 1100],'white')#opppenalty
  ax.plot([4500,3900],[-1100, -1100],'white')#opppenalty

  ax.plot([-3900,-3900],[-1100, 1100],'white') #ownpenalty
  ax.plot([-4500,-3900],[1100, 1100],'white')#ownpenalty
  ax.plot([-4500,-3900],[-1100, -1100],'white')#ownpenalty


  #Middle Circle
  ax.add_artist(Circle(xy=(0, 0),radius=750, fill=False, edgecolor='white'))
  #Penalty Marks
  ax.add_artist(Circle(xy=(4500-1300, 0),radius=50, color='white'))
  ax.add_artist(Circle(xy=(-4500+1300, 0),radius=50, color='white'))

  #OwnGoalBox -not quite correct - goalpostradius is not considered
  ax.plot([-4500,-5000],[-750, -750],'white')#ownpenalty
  ax.plot([-4500,-5000],[750, 750],'white')#ownpenalty
  ax.plot([-5000,-5000],[-750, 750],'white')#ownpenalty

  #OppGoalBox not quite correct - goalpostradius is not considered
  ax.plot([4500,5000],[-750, -750],'white')#ownpenalty
  ax.plot([4500,5000],[750, 750],'white')#ownpenalty
  ax.plot([5000,5000],[-750, 750],'white')#ownpenalty

  ax.set_xlim([-5200,5200])#Todo use FieldInfo Stuff
  ax.set_ylim([-3700,3700])
  ax.set_axis_bgcolor('green')
  ax.set_aspect("equal")

def distance(line):
    y = line.point2Y - line.point1Y
    x = line.point2X - line.point1X
    #y = line[1][1]-line[0][1]
    #x = line[1][0]-line[0][0]
    ans=y*y+x*x
    return math.sqrt(ans)
