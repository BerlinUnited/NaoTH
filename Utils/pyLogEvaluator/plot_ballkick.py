import sys
import math
import matplotlib
matplotlib.use('Qt4Agg')
#matplotlib.use('TkAgg')

from matplotlib.backends import qt_compat
from matplotlib import pyplot as plt

import math2d as m2d
from parse_behavior import BehaviorData, WorldState, Ball, Kick

import cPickle


def getBall(s):
  return s.robot_pose*s.ball.percept
  
if __name__ == "__main__":
  data = cPickle.load( open( "ballkick.cPickle", "r" ) )

  # first kick
  kick = data.kicks.values()[4]
  print kick.name
  
  #print data.state
  balls = []
  balls.extend([getBall(d) for d in data.state if d.frame > kick.frame and d.frame < kick.frame + 100 and d.ball.was_seen])
  
  x = [b.x for b in balls]
  y = [b.y for b in balls]

  #plt.plot(x,y)
  plt.plot(x,y)
  plt.gca().set_xlim((-4500, 4500))
  plt.gca().set_ylim((-3000, 3000))
  plt.show()
  