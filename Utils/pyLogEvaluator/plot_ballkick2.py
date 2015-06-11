import sys
import math
import matplotlib
from matplotlib import pyplot as plt 
import cPickle

import math2d as m2d
from parse_behavior import BehaviorData, WorldState, Ball, Kick


def getBall(s):
  return s.odometry*s.ball.percept

if __name__ == "__main__":
  data = cPickle.load( open( "ballkick.cPickle", "r" ) )

  trajectories = {}
  lastTime = 0
  origin = m2d.Pose2D()
  
  for kick in data.kicks.values():
    print kick.name
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
          print (ballpos-trajectory[-1]).abs()/(bp.time - lastTime)*1000 # in s
          if bp.frame < kick.frame + 120: #and math.hypot(ballpos.x-trajectory[-1][0], ballpos.y-trajectory[-1][1])/(bp[0] - lastFrame) > 1:
            trajectory.append(ballpos)
          else:
            break
        lastTime = bp.time
    trajectories[kick.name].append(trajectory)

  #"do_kick_with_left_foot","attack_with_left_foot","do_kick_with_right_foot","attack_with_right_foot"
  #"sidekick_to_right","sidekick_to_left"
  
  marker = {}
  marker["do_kick_with_left_foot"] = "v"
  marker["do_kick_with_right_foot"] = "v"
  marker["attack_with_left_foot"] = "o"
  marker["attack_with_right_foot"] = "o"
  marker["sidekick_to_right"] = '*'
  marker["sidekick_to_left"] = 's'
  
  #k = "sidekick_to_left"
  for k in trajectories.keys():  
    for trajectory in trajectories[k]:
      plt.plot([b.x for b in trajectory], [b.y for b in trajectory], marker[k])
  plt.gca().set_xlim((-500, 3000))
  plt.gca().set_ylim((-3000, 3000))
  plt.show()


