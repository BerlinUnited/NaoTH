import sys
import math
import matplotlib
from matplotlib import pyplot as plt 
import cPickle

import math2d as m2d
from parse_behavior import BehaviorData, WorldState, Ball, Kick

import numpy as np


def getBall(s):
  return s.odometry*s.ball.percept

if __name__ == "__main__":
  data = cPickle.load( open( "ballkick.cPickle", "r" ) )

  trajectories = {}
  lastTime = 0
  origin = m2d.Pose2D()

#  print data.kicks
  for kick in data.kicks.values():
#    print kick.name
    if not kick.name in trajectories:
      trajectories[kick.name] = []
    
    trajectory = []
    for bp in data.state:
      if bp.frame == kick.frame:
        origin = ~(bp.odometry)
      if bp.frame >= kick.frame: # frame number
        ballpos = origin*getBall(bp)
        if len(trajectory) == 0:
          trajectory.append(ballpos)
        else:
#          print (ballpos-trajectory[-1]).abs()/(bp.time - lastTime)*1000 # in s
          if bp.frame < kick.frame + 120: #and math.hypot(ballpos.x-trajectory[-1][0], ballpos.y-trajectory[-1][1])/(bp[0] - lastFrame) > 1:
            trajectory.append(ballpos)
          else:
            break
        lastTime = bp.time
    trajectories[kick.name].append(trajectory)

  #"do_kick_with_left_foot","attack_with_left_foot","do_kick_with_right_foot","attack_with_right_foot"
  #"sidekick_to_right","sidekick_to_left"
  
  marker = {}
  marker["do_kick_with_left_foot"] = "rv"
  marker["do_kick_with_right_foot"] = "bv"
  marker["attack_with_left_foot"] = "yo"
  marker["attack_with_right_foot"] = "co"
  marker["sidekick_to_right"] = 'm*'
  marker["sidekick_to_left"] = 'gs'
  
  #k = "sidekick_to_left"
  for k in trajectories.keys():  
    plt.clf()
    for trajectory in trajectories[k]:
      plt.subplot(211)
      plt.gca().set_xlim((-500, 3000))
      plt.gca().set_ylim((-3000, 3000))
      # plot original data
      plt.plot([b.x for b in trajectory], [b.y for b in trajectory], marker[k], mew=0)
      # fit line
      x = [b.x for b in trajectory]
      y = [b.y for b in trajectory]
      p = np.polyfit(x, y, 1)
      # plot fit
      xx = np.linspace(min(x)-200, max(x)+200, 100)
      yy = p[0]*xx+p[1]
      plt.plot(xx, yy, "k--", lw=1)
      # calculate speed
      v = np.hypot(np.diff(x), np.diff(y))
      plt.subplot(212)
      plt.plot(v)
      nMean = 30
      plt.plot(np.convolve(np.hanning(nMean)/np.hanning(nMean).sum(), np.r_[2*v[0]-v[nMean-1::-1],v,2*v[-1]-v[-1:-nMean:-1]], mode="same")[nMean:-nMean+1])
    plt.show()
