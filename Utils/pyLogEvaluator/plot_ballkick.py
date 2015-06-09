import sys
import math
import matplotlib
matplotlib.use('Qt4Agg')
#matplotlib.use('TkAgg')

from matplotlib.backends import qt_compat
from matplotlib import pyplot as plt

from parse_behavior import BehaviorData 
import cPickle

class Vector2:
  def __init__(self, x=0, y=0):
    self.x = x
    self.y = y

  def __add__(self, other):
    return Vector2(self.x + other.x, self.y + other.y)
    
  def __sub__(self, other):
    return Vector2(self.x - other.x, self.y - other.y)

  def __neg__(self):
    return Vector2(-self.x, -self.y)
    
  def rotate(self, a):
    return Vector2(
      math.cos(a)*self.x - math.sin(a)*self.y, 
      math.sin(a)*self.x + math.cos(a)*self.y)
    
class Pose2D:
  def __init__(self):
    self.translation = Vector2()
    self.rotation = 0
    
  def __mul__(self, other):
    return other.rotate(self.rotation) + self.translation
    
  def __invert__(self):
    p = Pose2D()
    p.rotation = -self.rotation
    p.translation = (Vector2() - self.translation).rotate(p.rotation)
    return p

class Point2D:
  def __init__(self):
    self.x = 0
    self.y = 0
    self.rx = 0
    self.ry = 0

def toGlobal(px, py, pr, x, y):
  p = Pose2D()
  p.translation = Vector2(px,py)
  p.rotation = pr
  
  b = Vector2(x,y)
 
  return p*b
  

def getBall(msg):
  px = msg["robot_pose.x"]
  py = msg["robot_pose.y"]
  pr = msg["robot_pose.rotation"]*math.pi/180 # hack
  x = msg["ball.percept.x"]
  y = msg["ball.percept.y"]
  return toGlobal(px, py, pr, x, y)
  
if __name__ == "__main__":
  data = cPickle.load( open( "ballkick.cPickle", "r" ) )

  # first kick
  frame = data.kicks[4][0]
  #print frame
  
  #print data.ballpose
  balls = []
  balls.extend([getBall(d[1]) for d in data.ballpose if d[0] > frame and d[0] < frame + 100 and d[1]["ball.was_seen"]])
  
  x = [b.x for b in balls]
  y = [b.y for b in balls]
  #x = [b.rx for b in balls]
  #y = [b.ry for b in balls]
  
  #plt.plot(x,y)
  plt.plot(x,y)
  plt.gca().set_xlim((-4500, 4500))
  plt.gca().set_ylim((-3000, 3000))
  plt.show()
  