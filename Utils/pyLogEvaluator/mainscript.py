import BehaviorParseTest

from matplotlib import pyplot
from matplotlib import colors

import numpy as np

import math2d as m2d

JointID = {}
JointID["HeadPitch"] = 0
JointID["HeadYaw"] = 1
JointID["RShoulderRoll"] = 2
JointID["LShoulderRoll"] = 3
JointID["RShoulderPitch"] = 4
JointID["LShoulderPitch"] = 5
JointID["RElbowRoll"] = 6
JointID["LElbowRoll"] = 7
JointID["RElbowYaw"] = 8
JointID["LElbowYaw"] = 9
JointID["RHipYawPitch"] = 10
JointID["LHipYawPitch"] = 11
JointID["RHipPitch"] = 12
JointID["LHipPitch"] = 13
JointID["RHipRoll"] = 14
JointID["LHipRoll"] = 15
JointID["RKneePitch"] = 16
JointID["LKneePitch"] = 17
JointID["RAnklePitch"] = 18
JointID["LAnklePitch"] = 19
JointID["RAnkleRoll"] = 20
JointID["LAnkleRoll"] = 21


def frameFilter(frame):
  m = None
  o = None
  try:
    
    if "BehaviorStateComplete" in frame.messages:
      m, o = frame["BehaviorStateComplete"]
    else:
      m, o = frame["BehaviorStateSparse"]

    kick = 0
    if "new_sidekick_right" in o and o["new_sidekick_right"] == 0:
      kick = 1
    elif "new_sidekick_left" in o and o["new_sidekick_left"] == 0:
      kick = 2
    elif "new_kick_long_with_approach" in o and o["new_kick_long_with_approach"] == 0:
      kick = 3
    elif "new_kick_short_with_approach" in o and o["new_kick_short_with_approach"] == 0:
      kick = 4
      
    return [frame["FrameInfo"].time/(1000.0*60), 
            # older logs don't have body status
            0,#frame["BodyStatus"].currentSum[JointID["RKneePitch"]], 
            0,#frame["BodyStatus"].currentSum[JointID["LKneePitch"]],
            0,#frame["BodyStatus"].currentSum[JointID["RAnklePitch"]], 
            0,#frame["BodyStatus"].currentSum[JointID["LAnklePitch"]],
            m["executed_motion.type"],
            m["ball.was_seen"],
            m["body.temperature.leg.left"],
            m["body.temperature.leg.right"],
            m["team.calc_if_is_striker"],
            m["game.state"],
            kick, #11
            m["robot_pose.x"], 
            m["robot_pose.y"],
            m["robot_pose.rotation"],
            m["ball.x"],
            m["ball.y"],
            m["ball.position.field.x"], #17
            m["ball.position.field.y"]
           ]
    
  except KeyError:
    raise StopIteration

def init():
    fileName = "./game.log"
    parser = BehaviorParseTest.BehaviorParser()
    log = BehaviorParseTest.LogReader(fileName, parser)
    
    # enforce the whole log being parsed (this is necessary for older game logs)
    for frame in log:
      if "BehaviorStateComplete" in frame.messages:
        m = frame["BehaviorStateComplete"]
      if "BehaviorStateSparse" in frame.messages:
        m = frame["BehaviorStateSparse"]
      frame["FrameInfo"]
      
    return log
    

def run(log):
    reload(BehaviorParseTest)
    
    # we want only the frames which contain BehaviorState*
    vlog = filter(lambda f: "BehaviorStateComplete" in f.messages or "BehaviorStateSparse" in f.messages, log)
    
    # apply the filter
    a = map(frameFilter, vlog)
    
    # make an numpy array
    data = np.array(a)

    
    stand = [x for x in data if x[5] == 4]
    walk = [x for x in data if x[5] == 5]

    #d_stand = zip(*stand)
    #d_walk = zip(*walk)
    
    #pyplot.plot(d_stand[0], d_stand[7], ".", label="RKneePitch-stand")
    #pyplot.plot(d_walk[0], d_walk[7], ".", label="RKneePitch-walk")

    bb = zip(*data)
    #pyplot.plot(bb[0], bb[8], label="temperatur right")
    #pyplot.plot(bb[0], bb[7], label="temperatur left")
    
    #pyplot.plot(bb[0], bb[10], label="game")
    #pyplot.plot(bb[0], bb[5], label="motion")
    #pyplot.plot(bb[0], bb[11], label="kicks")
    
    
    # kicks
    kick_data = []
    kick_data.append(filter(lambda m: m[11] == 1, data))
    kick_data.append(filter(lambda m: m[11] == 2, data))
    kick_data.append(filter(lambda m: m[11] == 3, data))
    kick_data.append(filter(lambda m: m[11] == 4, data))
    
    for kd in kick_data:
      kick_data_bb = np.array(zip(*kd))
      pyplot.plot(kick_data_bb[12], kick_data_bb[13], "bs", label="robot position")
      pyplot.plot(kick_data_bb[17], kick_data_bb[18], "o", label="ball position")
      U = kick_data_bb[17]-kick_data_bb[12]
      V = kick_data_bb[18]-kick_data_bb[13]
      D = np.sqrt(U*U+V*V)
      U = np.divide(U, D)
      V = np.divide(V, D)
      pyplot.quiver(kick_data_bb[12], kick_data_bb[13], U, V, color='lightgray', edgecolors=('k'), linewidths=(2,), headaxislength=5 )
    
    
    
    pyplot.legend(loc='upper left')
    pyplot.grid()
    pyplot.show()