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


def behavior(frame):
  m = None
  try:
    
    if "BehaviorStateComplete" in frame.messages:
      #print frame["BehaviorStateComplete"].decimal
      m = frame["BehaviorStateComplete"]
    else:
      m = frame["BehaviorStateSparse"]

    #return [m["robot_pose.x"], m["robot_pose.x"], m["fall_down_state"]]
    
    return [frame["FrameInfo"].time/(1000.0*60), 
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

def part1():
    parser = BehaviorParseTest.BehaviorParser()
    #fileName = "./game.log"
    #fileName = "D:\\RoboCup\\behavior.log\\rc14-game-htwk-qf\\2-half\\6026\\game.log"
    #fileName = "F:\\151004-1921-Nao6035\\game.log"
    fileName = "F:\\151004-1919-Nao6029\\game.log"
    fileName = "F:\\091201-0819-Nao6022\\game.log"
    log = BehaviorParseTest.LogReader(fileName, parser)
    for frame in log:
      if "BehaviorStateComplete" in frame.messages:
        m = frame["BehaviorStateComplete"]
      if "BehaviorStateSparse" in frame.messages:
        m = frame["BehaviorStateSparse"]
      frame["FrameInfo"]
      
    return log
    

def part2(log):
    reload(BehaviorParseTest)
    
    vlog = filter(lambda f: "BehaviorStateComplete" in f.messages or "BehaviorStateSparse" in f.messages, log)
    at = map(behavior, vlog)
    
    a = np.array(at)
    #da = np.diff(a, axis=1)
    
    stand = [x for x in a if x[5] == 4]
    walk = [x for x in a if x[5] == 5]

    r = []
    c = a[0][5]
    energy = 0
    f = 0
    for x in a:
      if c != x[5]:
        if x[5] == 5:
          energy = x[1]
          f = 1
        elif c == 5:
          r.append((x[1]-energy)/f)
        c = x[5]
      f = f+1
        
    #pyplot.plot(r, label="RKneePitch-stand")
    
    d_stand = zip(*stand)
    d_walk = zip(*walk)

    bb = zip(*a)
    pyplot.plot(bb[0], bb[8], label="temperatur right")
    pyplot.plot(bb[0], bb[7], label="temperatur left")
    pyplot.plot(bb[0], bb[10], label="game")
    pyplot.plot(bb[0], bb[5], label="motion")
    
    count = 0
    s = []
    for x in bb[9]:
      if x > 0:
        count = count + 1
      else:
        if count > 0:
          s.append(count/30)
        count = 0
    
    #pyplot.plot(s, ".", label="striker")
    
    #pyplot.plot(bb[0], bb[1]/max(bb[1]), label="current RKneePitch")

    #pyplot.plot(d_stand[0], d_stand[1], ".", label="RKneePitch-stand")
    #pyplot.plot(d_walk[0], d_walk[1], ".", label="RKneePitch-walk")
    
    #pyplot.plot(d_stand[0], d_stand[2], label="LKneePitch")
    #pyplot.plot(d_stand[0], d_stand[3], label="RAnklePitch")
    #pyplot.plot(d_stand[0], d_stand[4], label="LAnklePitch")
    pyplot.legend(loc='upper left')
    pyplot.grid()
    pyplot.show()