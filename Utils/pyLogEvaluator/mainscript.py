import BehaviorParseTest

from matplotlib import pyplot
import numpy as np

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
  try:
    
    if "BehaviorStateComplete" in frame.messages:
      m = frame["BehaviorStateComplete"]
    else:
      m = frame["BehaviorStateSparse"]

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
            m["game.state"]
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

    d_stand = zip(*stand)
    d_walk = zip(*walk)
    
    pyplot.plot(d_stand[0], d_stand[7], ".", label="RKneePitch-stand")
    pyplot.plot(d_walk[0], d_walk[7], ".", label="RKneePitch-walk")

    bb = zip(*data)
    #pyplot.plot(bb[0], bb[8], label="temperatur right")
    #pyplot.plot(bb[0], bb[7], label="temperatur left")
    #pyplot.plot(bb[0], bb[10], label="game")
    #pyplot.plot(bb[0], bb[5], label="motion")
    
    pyplot.legend(loc='upper left')
    pyplot.grid()
    pyplot.show()