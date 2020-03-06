from naoth.log import Reader as LogReader
from naoth.log import BehaviorParser

from matplotlib import pyplot
from mpl_toolkits.mplot3d import Axes3D
import numpy as np
import math

JointID = {
    "HeadPitch": 0,
    "HeadYaw": 1,
    "RShoulderRoll": 2,
    "LShoulderRoll": 3,
    "RShoulderPitch": 4,
    "LShoulderPitch": 5,
    "RElbowRoll": 6,
    "LElbowRoll": 7,
    "RElbowYaw": 8,
    "LElbowYaw": 9,
    "RHipYawPitch": 10,
    "LHipYawPitch": 11,
    "RHipPitch": 12,
    "LHipPitch": 13,
    "RHipRoll": 14,
    "LHipRoll": 15,
    "RKneePitch": 16,
    "LKneePitch": 17,
    "RAnklePitch": 18,
    "LAnklePitch": 19,
    "RAnkleRoll": 20,
    "LAnkleRoll": 21
}

JointNames = {
    0: "HeadPitch",
    1: "HeadYaw",
    2: "RShoulderRoll",
    3: "LShoulderRoll",
    4: "RShoulderPitch",
    5: "LShoulderPitch",
    6: "RElbowRoll",
    7: "LElbowRoll",
    8: "RElbowYaw",
    9: "LElbowYaw",
    10: "RHipYawPitch",
    11: "LHipYawPitch",
    12: "RHipPitch",
    13: "LHipPitch",
    14: "RHipRoll",
    15: "LHipRoll",
    16: "RKneePitch",
    17: "LKneePitch",
    18: "RAnklePitch",
    19: "LAnklePitch",
    20: "RAnkleRoll",
    21: "LAnkleRoll",
}


def frame_filter(frame):
    try:
        val = [frame["FrameInfo"].time / 1000.0,
               frame["MotionRequest"],
               frame["MotorJointData"],
               frame["SensorJointData"]
               ]
        # print val
        return val

    except KeyError:
        raise StopIteration


def init():
    file_name = "/home/steffen/NaoTH/Logs/Motion/"
    file_name = file_name + ""
    # file_name = file_name + "/walk_forward.log"
    file_name = file_name + "/walk_backward.log"
    parser = BehaviorParser()
    parser.register("MotorJointData", "JointData")
    log = LogReader(file_name, parser)

    # enforce the whole log being parsed (this is necessary for older game logs)
    for frame in log:
        if "BehaviorStateComplete" in frame.messages:
            m = frame["BehaviorStateComplete"]
        if "BehaviorStateSparse" in frame.messages:
            m = frame["BehaviorStateSparse"]
        frame["FrameInfo"]

    return log


def run(log):

    print(log.names)

    # we want only the frames which contain BehaviorState*
    # vlog = filter(lambda f: "BehaviorStateComplete" in f.messages or "BehaviorStateSparse" in f.messages, log)

    # apply the filter
    b = map(frame_filter, log)

    for x in range(10, 21, 2):
        pyplot.close('all')

        a = [[
            m[0],  # time
            m[2].position[x],  # motor  position R
            m[3].jointData.position[x],  # sensor position R
            m[2].position[x + 1],  # motor  position L
            m[3].jointData.position[x + 1],  # sensor position L
            m[3].temperature[x],  # temp R
            m[3].temperature[x + 1],  # temp L
            m[3].electricCurrent[x],  # current R
            m[3].electricCurrent[x + 1]  # current L
        ] for m in b]

        # make an numpy array
        data = np.array(a)

        size = data.shape

        data2 = np.array(zip(*data))

        f, axarr = pyplot.subplots(3, sharex=True)

        d1 = np.append([0, 0, 0, 0], data2[1])
        d2 = np.append(data2[2], [0, 0, 0, 0])
        diff = abs(d2 - d1)
        print(JointNames[x] + ": {}".format(np.amax(diff[4:-4]) * 180 / math.pi))

        # pyplot.plot(d1)
        # pyplot.plot(d2)
        axarr[0].set_title('cum_error')
        axarr[0].plot(np.cumsum(diff), label=JointNames[x])

        d1 = np.append([0, 0, 0, 0], data2[3])
        d2 = np.append(data2[4], [0, 0, 0, 0])
        diff = abs(d2 - d1)
        print(JointNames[x + 1] + ": {}".format(np.amax(diff[4:-4]) * 180 / math.pi))

        # pyplot.plot(d1)
        # pyplot.plot(d2)
        axarr[0].plot(np.cumsum(diff), label=JointNames[x + 1])
        axarr[0].legend(loc='upper left')

        # d1 = np.append(data2[5], [0,0,0,0])
        # d2 = np.append(data2[6], [0,0,0,0])
        d1 = data2[5]
        d2 = data2[6]
        axarr[2].set_title('temperature')
        axarr[2].plot(d1, label=JointNames[x])
        axarr[2].plot(d2, label=JointNames[x + 1])
        axarr[2].legend(loc='upper left')

        axarr[1].set_title('cum_current')
        axarr[1].plot(np.cumsum(data2[7]), label=JointNames[x])
        axarr[1].plot(np.cumsum(data2[8]), label=JointNames[x + 1])
        axarr[1].legend(loc='upper left')

        '''
        tr_walk  = zip(*walk)
        
        time_stand = sum(tr_stand[0])
        time_walk  = sum(tr_walk[0])
        
        print "------------------"
        print "time standing: {}".format(time_stand)
        print "time walking : {}".format(time_walk)
        print "time total   : {}".format(time_stand + time_walk)
    
        # LLeg
        d_stand_LLeg_stats = do_statistics(tr_stand[0], tr_stand[1])
        print_statistics("left leg - stand", d_stand_LLeg_stats)
    
        d_walk_LLeg_stats = do_statistics(tr_walk[0], tr_walk[1])
        print_statistics("left leg - walk", d_walk_LLeg_stats)
    
        # RLeg
        d_stand_RLeg_stats = do_statistics(tr_stand[0], tr_stand[2])
        print_statistics("right leg - stand", d_stand_RLeg_stats)
    
        d_walk_RLeg_stats = do_statistics(tr_walk[0], tr_walk[2])
        print_statistics("right leg - walk", d_walk_RLeg_stats)
    
        bb = zip(*data)
        pyplot.plot(bb[0], bb[2], label="temperatur right")
        pyplot.plot(bb[0], bb[1], label="temperatur left")
        pyplot.plot(bb[0], bb[10], label="game")
        pyplot.plot(bb[0], bb[5], label="motion")
        
        pyplot.legend(loc='upper left')
        pyplot.grid()
        '''
        pyplot.show()
