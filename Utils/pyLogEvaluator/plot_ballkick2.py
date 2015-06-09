import sys
import math
import matplotlib
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

  trajectories = []

  for kick in data.kicks:
    print kick
    trajectory = []
    for bp in data.ballpose:
      if bp[0] > kick[0]:
        ballpos = getBall(bp[1])
        if len(trajectory) == 0:
          trajectory.append([ballpos.x, ballpos.y])
        else:
          if math.hypot(ballpos.x-trajectory[-1][0], ballpos.y-trajectory[-1][1]) < 1000:
            trajectory.append([ballpos.x, ballpos.y])
    trajectories.append(trajectory)

  for trajectory in trajectories:
    plt.plot([x for x,y in trajectory], [y for x,y in trajectory], ".")
  plt.gca().set_xlim((-4500, 4500))
  plt.gca().set_ylim((-3000, 3000))
  plt.show()


