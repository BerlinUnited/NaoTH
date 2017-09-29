import BehaviorParser

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

# used for easier indexing of the data matrix
TimeIdx = 0

CurrentIdx = {}
CurrentIdx["RHipYawPitch"] = 1
CurrentIdx["LHipYawPitch"] = 2
CurrentIdx["RHipPitch"]    = 3
CurrentIdx["LHipPitch"]    = 4
CurrentIdx["RHipRoll"]     = 5
CurrentIdx["LHipRoll"]     = 6
CurrentIdx["RKneePitch"]   = 7
CurrentIdx["LKneePitch"]   = 8
CurrentIdx["RAnklePitch"]  = 9
CurrentIdx["LAnklePitch"]  = 10
CurrentIdx["RAnkleRoll"]   = 11
CurrentIdx["LAnkleRoll"]   = 12

TemperatureIdx = {}
TemperatureIdx["RHipYawPitch"] = 13
TemperatureIdx["LHipYawPitch"] = 14
TemperatureIdx["RHipPitch"]    = 15
TemperatureIdx["LHipPitch"]    = 16
TemperatureIdx["RHipRoll"]     = 17
TemperatureIdx["LHipRoll"]     = 18
TemperatureIdx["RKneePitch"]   = 19
TemperatureIdx["LKneePitch"]   = 20
TemperatureIdx["RAnklePitch"]  = 21
TemperatureIdx["LAnklePitch"]  = 22
TemperatureIdx["RAnkleRoll"]   = 23
TemperatureIdx["LAnkleRoll"]   = 24

CurrentMotionIdx = 25

def frameFilter(frame):
  try:
    
    return [frame["FrameInfo"].time/(1000.0), 
            frame["BodyStatus"].currentSum[JointID["RHipYawPitch"]],
            frame["BodyStatus"].currentSum[JointID["LHipYawPitch"]],
            frame["BodyStatus"].currentSum[JointID["RHipPitch"]],
            frame["BodyStatus"].currentSum[JointID["LHipPitch"]],
            frame["BodyStatus"].currentSum[JointID["RHipRoll"]],
            frame["BodyStatus"].currentSum[JointID["LHipRoll"]],
            frame["BodyStatus"].currentSum[JointID["RKneePitch"]], 
            frame["BodyStatus"].currentSum[JointID["LKneePitch"]],
            frame["BodyStatus"].currentSum[JointID["RAnklePitch"]], 
            frame["BodyStatus"].currentSum[JointID["LAnklePitch"]],
            frame["BodyStatus"].currentSum[JointID["RAnkleRoll"]],
            frame["BodyStatus"].currentSum[JointID["LAnkleRoll"]],
            
            frame["SensorJointData"].temperature[JointID["RHipYawPitch"]],
            frame["SensorJointData"].temperature[JointID["LHipYawPitch"]],
            frame["SensorJointData"].temperature[JointID["RHipPitch"]],
            frame["SensorJointData"].temperature[JointID["LHipPitch"]],
            frame["SensorJointData"].temperature[JointID["RHipRoll"]],
            frame["SensorJointData"].temperature[JointID["LHipRoll"]],
            frame["SensorJointData"].temperature[JointID["RKneePitch"]], 
            frame["SensorJointData"].temperature[JointID["LKneePitch"]],
            frame["SensorJointData"].temperature[JointID["RAnklePitch"]], 
            frame["SensorJointData"].temperature[JointID["LAnklePitch"]],
            frame["SensorJointData"].temperature[JointID["RAnkleRoll"]],
            frame["SensorJointData"].temperature[JointID["LAnkleRoll"]],
            
            frame["MotionStatus"].currentMotion 
           ]
    
  except KeyError:
    print "error while applying the frameFilter"
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
    #print "var  (time weighted): {}".format(stats[1])
    print "std  (time weighted): {}".format(stats[2])
    #print "mean: {}".format(stats[3])
    #print "var : {}".format(stats[4])
    #print "std : {}".format(stats[5])
    
def plot(data, joints, rows, cols):
    fig, ax = pyplot.subplots(rows, cols)
    ax = [axis for vertical_sub_plot in ax for axis in vertical_sub_plot]
    for (ax1,joint) in zip(ax,joints):
        ax1.plot(data[0][TimeIdx], data[0][CurrentIdx[joint]], "b")
        ax1.plot(data[1][TimeIdx], data[1][CurrentIdx[joint]], "b--")
        ax1.set_xlabel('time (s)')
        ax1.set_ylabel('cumulative current (A)')
        for tl in ax1.get_yticklabels():
            tl.set_color('b')
        
        ax2 = ax1.twinx()
        ax2.plot(data[0][TimeIdx], data[0][TemperatureIdx[joint]], "r", label=joint+" standard")
        ax2.plot(data[1][TimeIdx], data[1][TemperatureIdx[joint]], "r--", label=joint+" simple stiff joint control")
        ax2.set_ylabel('temperature (Celsius)')
        for tl in ax2.get_yticklabels():
            tl.set_color('r')
    
        pyplot.legend(loc='upper left')
        pyplot.grid()
    pyplot.show()


def init():
    parser = BehaviorParser.BehaviorParser()
    
    fileNamePrefix = "/home/steffen/NaoTH/Logs/EuropeanOpen16/2016-03-30-test-naodevils/half1/160330-1952-Nao6022"
    fileName = fileNamePrefix + "/game.log"
    log0 = BehaviorParser.LogReader(fileName, parser)
    
    fileNamePrefix = "/home/steffen/NaoTH/Logs/MesseLeipzig/lm15-naodevils-2/091201-0819-Nao6022"
    fileName = fileNamePrefix + "/game.log"
    log1 = BehaviorParser.LogReader(fileName, parser)

    return [log0, log1]
    

def run(log):
    reload(BehaviorParser)

    logs = log

    print "representations check --- start"

    print "contained representations"
    i = 0
    for log in logs:
        i = i+1
        print "log no.{}:".format(i)
        print log.names

    # check if all required representations are contained in log
    requiredReps = ['BodyStatus', 'FrameInfo', 'MotionStatus', 'OffsetJointData', 'SensorJointData']
    
    missingRep = False
    for rep in requiredReps:
        i = 0
        for log in logs:
            i = i+1
            if(rep not in log.names):
                print "ERROR: representation " + rep + " is not contained in log no.{} this log file but required by this script".format(i)
                missingRep = True

    if missingRep:
        return

    print "representations check --- done"

    print "data filtering --- start"

    # apply the filter
    a = map(frameFilter, logs[0])
    b = map(frameFilter, logs[1])

    print "data filtering --- done"

    data = [np.array(zip(*a)), np.array(zip(*b))]

    print "bias removing --- start"

    # remove bias from current
    for d in data:
        for idx in CurrentIdx.itervalues():
            d[idx] = d[idx] - d[idx][0]

    # remove bias from time
    for d in data:
        d[TimeIdx] = d[TimeIdx] - d[TimeIdx][0]
    
    print "bias removing --- done"

    plot(data,("RHipPitch","LHipPitch","RKneePitch","LKneePitch","RAnklePitch","LAnklePitch"),3,2)
    #for k in CurrentIdx:
    #    plot(data,k)
