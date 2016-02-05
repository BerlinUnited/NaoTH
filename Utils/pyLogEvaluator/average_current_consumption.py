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
  o = None
  try:
    
    if "BehaviorStateComplete" in frame.messages:
      m, o = frame["BehaviorStateComplete"]
    else:
      m, o = frame["BehaviorStateSparse"]

    return [frame["FrameInfo"].time/(1000.0), 
            # older logs don't have body status
            frame["BodyStatus"].currentSum[JointID["RKneePitch"]], 
            frame["BodyStatus"].currentSum[JointID["LKneePitch"]],
            frame["BodyStatus"].currentSum[JointID["RAnklePitch"]], 
            frame["BodyStatus"].currentSum[JointID["LAnklePitch"]],
            m["executed_motion.type"]
           ]
    
  except KeyError:
    raise StopIteration

def doStatistics(time, data):
    time_total = sum(time)

    d_data = map(lambda c,t: c/t, data, time)

    weighted_mean = np.average(d_data, axis = 0, weights = time/time_total)
    weighted_var  = np.average((d_data-weighted_mean)**2, axis = 0, weights = time/time_total)
    weighted_std  = np.sqrt(weighted_var)
    return [weighted_mean,
            weighted_var,
            weighted_std,
            np.mean(d_data),
            np.var(d_data),
            np.std(d_data)
           ]

def printStatistics(title,stats):
    print "------------------"
    print title
    print "mean (time weighted): {}".format(stats[0])
    print "var  (time weighted): {}".format(stats[1])
    print "std  (time weighted): {}".format(stats[2])
    print "mean: {}".format(stats[3])
    print "var : {}".format(stats[4])
    print "std : {}".format(stats[5])
    

def init():
    fileName = "/home/steffen/NaoTH/Logs/MesseLeipzig/lm15-naodevils-2/"
    fileName = fileName + "151004-1919-Nao6029"
    fileName = fileName + "/game.log"
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

    print log.names

    # we want only the frames which contain BehaviorState*
    vlog = filter(lambda f: "BehaviorStateComplete" in f.messages or "BehaviorStateSparse" in f.messages, log)
    
    # apply the filter
    a = map(frameFilter, vlog)
   
    # make an numpy array
    data = np.array(a)
    
    size = data.shape

    data2 = np.empty([0,6])

    current = 0
    for x in xrange(1,size[0]) :
        if data[current,5] == data[x,5] :
            continue
        elif (data[current,5] == 4 or data[current,5] == 5) :
            temp = data[x,:]-data[current,:]
            temp[5] = data[current,5]
            current = x
            data2 = np.vstack((data2,temp))
        else :
            current = x

    stand = [x for x in data2 if x[5] == 4]
    walk = [x for x in data2 if x[5] == 5]

    tr_stand = zip(*stand)
    tr_walk  = zip(*walk)
    
    time_stand = sum(tr_stand[0])
    time_walk = sum(tr_walk[0])
    
    print "------------------"
    print "time standing: {}".format(time_stand)
    print "time walking : {}".format(time_walk)
    print "time total   : {}".format(time_stand + time_walk)

    #RKneePitch
    d_stand_RKneePitch_stats = doStatistics(tr_stand[0], tr_stand[1])
    printStatistics("RKneePitch - stand", d_stand_RKneePitch_stats)

    d_walk_RKneePitch_stats = doStatistics(tr_walk[0], tr_walk[1])
    printStatistics("RKneePitch - walk", d_walk_RKneePitch_stats)

    #LKneePitch
    d_stand_LKneePitch_stats = doStatistics(tr_stand[0], tr_stand[2])
    printStatistics("LKneePitch - stand", d_stand_LKneePitch_stats)

    d_walk_LKneePitch_stats = doStatistics(tr_walk[0], tr_walk[2])
    printStatistics("LKneePitch - walk", d_walk_LKneePitch_stats)

    #RAnklePitch
    d_stand_RAnklePitch_stats = doStatistics(tr_stand[0], tr_stand[3])
    printStatistics("RAnklePitch - stand", d_stand_RAnklePitch_stats)

    d_walk_RAnklePitch_stats = doStatistics(tr_walk[0], tr_walk[3])
    printStatistics("RAnklePitch - walk", d_walk_RAnklePitch_stats)
   
    #LAnklePitch
    d_stand_LAnklePitch_stats = doStatistics(tr_stand[0], tr_stand[4])
    printStatistics("LAnklePitch - stand", d_stand_LAnklePitch_stats)

    d_walk_LAnklePitch_stats = doStatistics(tr_walk[0], tr_walk[4])
    printStatistics("LAnklePitch - walk", d_walk_LAnklePitch_stats)
   
   
    stand2 = [x for x in data if x[5] == 4]
    walk2 = [x for x in data if x[5] == 5]

    d_stand = zip(*stand2)
    d_walk = zip(*walk2)
    
    pyplot.plot(d_stand[0], d_stand[1], ".", label="RKneePitch-stand")
    pyplot.plot(d_walk[0], d_walk[1], ".", label="RKneePitch-walk")

    pyplot.plot(d_stand[0], d_stand[2], ".", label="LKneePitch-stand")
    pyplot.plot(d_walk[0], d_walk[2], ".", label="LKneePitch-walk")

    pyplot.plot(d_stand[0], d_stand[3], ".", label="RAnklePitch-stand")
    pyplot.plot(d_walk[0], d_walk[3], ".", label="RAnklePitch-walk")
    
    pyplot.plot(d_stand[0], d_stand[4], ".", label="LAnklePitch-stand")
    pyplot.plot(d_walk[0], d_walk[4], ".", label="LAnklePitch-walk")
    #bb = zip(*data)
    #pyplot.plot(bb[0], bb[8], label="temperatur right")
    #pyplot.plot(bb[0], bb[7], label="temperatur left")
    #pyplot.plot(bb[0], bb[10], label="game")
    #pyplot.plot(bb[0], bb[5], label="motion")
    
    pyplot.legend(loc='upper left')
    pyplot.grid()
    pyplot.show()
