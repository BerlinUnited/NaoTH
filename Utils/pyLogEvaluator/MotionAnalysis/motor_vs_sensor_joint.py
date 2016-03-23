import BehaviorParseTest

from matplotlib import pyplot
from mpl_toolkits.mplot3d import Axes3D
import numpy as np
import math

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

JointNames = {}
JointNames[0]  = "HeadPitch"
JointNames[1]  = "HeadYaw"
JointNames[2]  = "RShoulderRoll"
JointNames[3]  = "LShoulderRoll"
JointNames[4]  = "RShoulderPitch"
JointNames[5]  = "LShoulderPitch"
JointNames[6]  = "RElbowRoll"
JointNames[7]  = "LElbowRoll"
JointNames[8]  = "RElbowYaw"
JointNames[9]  = "LElbowYaw"
JointNames[10] = "RHipYawPitch" 
JointNames[11] = "LHipYawPitch" 
JointNames[12] = "RHipPitch"
JointNames[13] = "LHipPitch"
JointNames[14] = "RHipRoll"
JointNames[15] = "LHipRoll"
JointNames[16] = "RKneePitch"
JointNames[17] = "LKneePitch"
JointNames[18] = "RAnklePitch"
JointNames[19] = "LAnklePitch"
JointNames[20] = "RAnkleRoll"
JointNames[21] = "LAnkleRoll"

def frameFilter(frame):
  try:
    val =  [frame["FrameInfo"].time/(1000.0), 
            frame["MotionRequest"],
            frame["MotorJointData"],
            frame["SensorJointData"]
           ]

#    print val
    return val
           
    
  except KeyError:
    raise StopIteration

def init():
    fileName = "/home/steffen/NaoTH/Logs/Motion/"
    fileName = fileName + ""
    #fileName = fileName + "/walk_forward.log"
    fileName = fileName + "/walk_backward.log"
    parser = BehaviorParseTest.BehaviorParser()
    parser.register("MotorJointData","JointData")
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
    # vlog = filter(lambda f: "BehaviorStateComplete" in f.messages or "BehaviorStateSparse" in f.messages, log)
  
    # apply the filter
    b = map(frameFilter, log)
    
    for x in range(10,21,2):

        pyplot.close('all')

        a = [[
            m[0],                         #time
            m[2].position[x],             #motor  position R
            m[3].jointData.position[x],   #sensor position R
            m[2].position[x+1],           #motor  position L
            m[3].jointData.position[x+1], #sensor position L
            m[3].temperature[x],          #temp R
            m[3].temperature[x+1],        #temp L
            m[3].electricCurrent[x],      #current R
            m[3].electricCurrent[x+1]     #current L
        ] for m in b]
    
        # make an numpy array
        data = np.array(a)
        
        size = data.shape
    
        data2 = np.array(zip(*data))
        
        f, axarr = pyplot.subplots(3, sharex=True)
        
        d1 = np.append([0,0,0,0], data2[1])
        d2 = np.append(data2[2], [0,0,0,0])
        diff = abs(d2-d1)
        print JointNames[x] + ": {}".format(np.amax(diff[4:-4])*180/math.pi)
        
        #pyplot.plot(d1)
        #pyplot.plot(d2)
        axarr[0].set_title('cum_error')
        axarr[0].plot(np.cumsum(diff), label = JointNames[x])
    
        d1 = np.append([0,0,0,0], data2[3])
        d2 = np.append(data2[4], [0,0,0,0])
        diff = abs(d2-d1)
        print JointNames[x+1] + ": {}".format(np.amax(diff[4:-4])*180/math.pi)
    
        #pyplot.plot(d1)
        #pyplot.plot(d2)
        axarr[0].plot(np.cumsum(diff), label = JointNames[x+1])
        axarr[0].legend(loc='upper left')
    
        #d1 = np.append(data2[5], [0,0,0,0])
        #d2 = np.append(data2[6], [0,0,0,0])
        d1 = data2[5]
        d2 = data2[6]
        axarr[2].set_title('temperature')
        axarr[2].plot(d1,label = JointNames[x])
        axarr[2].plot(d2,label = JointNames[x+1])
        axarr[2].legend(loc='upper left')
   
        axarr[1].set_title('cum_current')
        axarr[1].plot(np.cumsum(data2[7]), label = JointNames[x])
        axarr[1].plot(np.cumsum(data2[8]), label = JointNames[x+1])
        axarr[1].legend(loc='upper left')
    
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
        
        #pyplot.legend(loc='upper left')
        #pyplot.grid()
        pyplot.show()
