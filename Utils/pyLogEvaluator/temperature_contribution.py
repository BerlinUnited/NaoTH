import BehaviorParseTest

from matplotlib import pyplot
from mpl_toolkits.mplot3d import Axes3D
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
            m["body.temperature.leg.left"],
            m["body.temperature.leg.right"],
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
    
def reduceData(data):
    time = data[0][-1] - data[0][0]
    temp = data[1][0]
    ratio_stand =data[2].count(4.0)/float(len(data[2]))
    ratio_walk  =data[2].count(5.0)/float(len(data[2]))
    return [time, temp, ratio_stand, ratio_walk]

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

    #print data

    data_left  = [[]]
    data_right = [[]]

    temp_left  = data[0,1]
    temp_right = data[0,2]

    for x in data:
        # rearrange for left leg
        if temp_left != x[1]:
            # start a new sub-list
            data_left.append([[x[0],x[1],x[3]]])
            temp_left = x[1]
        else: 
            # append to the last sub-list of r
            data_left[-1].append([x[0],x[1],x[3]])

        # rearrange for right leg
        if temp_right != x[2]:
            # start a new sub-list
            data_right.append([[x[0],x[2],x[3]]])
            temp_right = x[2]
        else: 
            # append to the last sub-list of r
            data_right[-1].append([x[0],x[2],x[3]])

    tr_data_left  = map(lambda d: zip(*d),data_left)
    tr_data_right = map(lambda d: zip(*d),data_right)
    
    data_left_reduced  = map(lambda d: reduceData(d),tr_data_left)
    data_right_reduced = map(lambda d: reduceData(d),tr_data_right)
    
    data_left_only_stand = [x for x in data_left_reduced if x[2] == 1.0]
    data_left_only_walk  = [x for x in data_left_reduced if x[3] == 1.0]

    print "Left Leg"
    print "------------"
    print "Number of only-stand-degrees: {}".format(len(data_left_only_stand))
    print "Number of only-walk-degress : {}".format(len(data_left_only_walk))

    print "------------"
    print "duration of the only-walk-degress"
    for x in data_left_only_walk:
        print x
    
    print ""

    data_right_only_stand = [x for x in data_right_reduced if x[2] == 1.0]
    data_right_only_walk  = [x for x in data_right_reduced if x[3] == 1.0]
    
    print "Right Leg"
    print "------------"
    print "Number of only-stand-degrees: {}".format(len(data_right_only_stand))
    print "Number of only-walk-degress : {}".format(len(data_right_only_walk))

    print "------------"
    print "duration of the only-walk-degress"
    for x in data_right_only_walk:
        print x
    
    print ""
    
    data_scatter_left = zip(*data_left_reduced)
    #pyplot.scatter(data_scatter_left[2], data_scatter_left[3], c=data_scatter_left[0], marker='o')

    data_scatter_right = zip(*data_right_reduced)
    #pyplot.scatter(data_scatter_right[2], data_scatter_right[3], c=data_scatter_right[0], marker='o')
    
    data_scatter = [data_scatter_left[0] + data_scatter_right[0],
            data_scatter_left[1] + data_scatter_right[1],
            data_scatter_left[2] + data_scatter_right[2],
            data_scatter_left[3] + data_scatter_right[3]]    

    pyplot.scatter(data_scatter[2], data_scatter[3], c=data_scatter[0], marker='o')

    #tr_walk  = zip(*walk)
    #
    #time_stand = sum(tr_stand[0])
    #time_walk  = sum(tr_walk[0])
    #
    #print "------------------"
    #print "time standing: {}".format(time_stand)
    #print "time walking : {}".format(time_walk)
    #print "time total   : {}".format(time_stand + time_walk)

    ##LLeg
    #d_stand_LLeg_stats = doStatistics(tr_stand[0], tr_stand[1])
    #printStatistics("left leg - stand", d_stand_LLeg_stats)

    #d_walk_LLeg_stats = doStatistics(tr_walk[0], tr_walk[1])
    #printStatistics("left leg - walk", d_walk_LLeg_stats)

    ##RLeg
    #d_stand_RLeg_stats = doStatistics(tr_stand[0], tr_stand[2])
    #printStatistics("right leg - stand", d_stand_RLeg_stats)

    #d_walk_RLeg_stats = doStatistics(tr_walk[0], tr_walk[2])
    #printStatistics("right leg - walk", d_walk_RLeg_stats)

    #bb = zip(*data)
    #pyplot.plot(bb[0], bb[2], label="temperatur right")
    #pyplot.plot(bb[0], bb[1], label="temperatur left")
    #pyplot.plot(bb[0], bb[10], label="game")
    #pyplot.plot(bb[0], bb[5], label="motion")
    
    pyplot.legend(loc='upper left')
    pyplot.ylabel('ratio walk')
    pyplot.xlabel('ratio stand')
    pyplot.axis([0,1,0,1])
    #pyplot.grid()
    pyplot.show()
