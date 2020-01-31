#import BehaviorParser
from naoth.log import Reader as LogReader
from naoth.log import BehaviorParser
from matplotlib import pyplot
import numpy as np

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


def frame_filter(frame):
    m = None
    o = None
    try:
        if "BehaviorStateComplete" in frame.messages:
            m, o = frame["BehaviorStateComplete"]
        else:
            m, o = frame["BehaviorStateSparse"]

        print(frame["BodyStatus"].currentSum[JointID["RHipYawPitch"]])
        return [frame["FrameInfo"].time/1000.0,
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
                m["executed_motion.type"]
                ]

    except KeyError:
        raise StopIteration


def do_statistics(time, data):
    time_total = sum(time)

    d_data = map(lambda c, t: c/t, data, time)

    weighted_mean = np.average(d_data, axis=0, weights=time/time_total)
    weighted_var = np.average((d_data - weighted_mean)**2, axis=0, weights=time/time_total)
    weighted_std = np.sqrt(weighted_var)
    return [weighted_mean,
            weighted_var,
            weighted_std,
            np.mean(d_data),
            np.var(d_data),
            np.std(d_data)
            ]


def print_statistics(title, stats):
    print("------------------")
    print(title)
    print("mean (time weighted): {}".format(stats[0]))
    # print "var  (time weighted): {}".format(stats[1])
    print("std  (time weighted): {}".format(stats[2]))
    # print "mean: {}".format(stats[3])
    # print "var : {}".format(stats[4])
    # print "std : {}".format(stats[5])
    

def init():
    # file_name = "/home/steffen/NaoTH/Logs/MesseLeipzig/lm15-naodevils-2/"
    # file_name = file_name + "091201-0819-Nao6022"
    #file_name = "/home/steffen/NaoTH/Logs/EuropeanOpen16/2016-03-30-test-naodevils/half1/"
    #file_name = file_name + "160330-1952-Nao6022"
    #file_name = file_name + "/game.log"
    file_name = "game.log"
    parser = BehaviorParser()
    log = LogReader(file_name, parser)
    
    # enforce the whole log being parsed (this is necessary for older game logs)
    for frame in log:
        pass
        #print(frame.number)
        #if "BehaviorStateComplete" in frame.messages:
        #    m = frame["BehaviorStateComplete"]
        #if "BehaviorStateSparse" in frame.messages:
        #    m = frame["BehaviorStateSparse"]
        #frame["FrameInfo"]
      
    return log
    

def run(log):
    #reload(BehaviorParser)

    print(log.names)

    for i in log.messages:
        print(i)
    quit()
    # we want only the frames which contain BehaviorState*
    vlog = filter(lambda f: "BehaviorStateComplete" in f.messages or "BehaviorStateSparse" in f.messages, log)
   
    # apply the filter
    a = map(frame_filter, vlog)
   
    # make an numpy array
    data = np.array(a)
    print(vlog)
    size = data.shape

    data2 = np.empty([0, 14])
    quit()
    current = 0
    motiontypeIndex = 13
    for x in range(1, size[0]):
        if data[current, motiontypeIndex] == data[x, motiontypeIndex]:
            continue
        elif data[current, motiontypeIndex] == 4 or data[current, motiontypeIndex] == 5:
            temp = data[x, :]-data[current, :]
            temp[motiontypeIndex] = data[current, motiontypeIndex]
            current = x
            data2 = np.vstack((data2, temp))
        else:
            current = x
    
    stand = [x for x in data2 if x[motiontypeIndex] == 4]
    walk = [x for x in data2 if x[motiontypeIndex] == 5]

    tr_stand = zip(*stand)
    tr_walk = zip(*walk)
    
    time_stand = sum(tr_stand[0])
    time_walk = sum(tr_walk[0])
    
    print("------------------")
    print("time standing: {}".format(time_stand))
    print("time walking : {}".format(time_walk))
    print("time total   : {}".format(time_stand + time_walk))

    # RHipYawPitch
    d_stand_RHipYawPitch_stats = do_statistics(tr_stand[0], tr_stand[1])
    print_statistics("RHipYawPitch - stand", d_stand_RHipYawPitch_stats)

    d_walk_RHipYawPitch_stats = do_statistics(tr_walk[0], tr_walk[1])
    print_statistics("RHipYawPitch - walk", d_walk_RHipYawPitch_stats)

    # LHipYawPitch
    d_stand_LHipYawPitch_stats = do_statistics(tr_stand[0], tr_stand[2])
    print_statistics("LHipYawPitch - stand", d_stand_LHipYawPitch_stats)

    d_walk_LHipYawPitch_stats = do_statistics(tr_walk[0], tr_walk[2])
    print_statistics("LHipYawPitch - walk", d_walk_LHipYawPitch_stats)

    # RHipPitch
    d_stand_RHipPitch_stats = do_statistics(tr_stand[0], tr_stand[3])
    print_statistics("RHipPitch - stand", d_stand_RHipPitch_stats)

    d_walk_RHipPitch_stats = do_statistics(tr_walk[0], tr_walk[3])
    print_statistics("RHipPitch - walk", d_walk_RHipPitch_stats)

    # LHipPitch
    d_stand_LHipPitch_stats = do_statistics(tr_stand[0], tr_stand[4])
    print_statistics("LHipPitch - stand", d_stand_LHipPitch_stats)

    d_walk_LHipPitch_stats = do_statistics(tr_walk[0], tr_walk[4])
    print_statistics("LHipPitch - walk", d_walk_LHipPitch_stats)

    # RHipRoll
    d_stand_RHipRoll_stats = do_statistics(tr_stand[0], tr_stand[5])
    print_statistics("RHipRoll - stand", d_stand_RHipRoll_stats)

    d_walk_RHipRoll_stats = do_statistics(tr_walk[0], tr_walk[5])
    print_statistics("RHipRoll - walk", d_walk_RHipRoll_stats)

    # LHipRoll
    d_stand_LHipRoll_stats = do_statistics(tr_stand[0], tr_stand[6])
    print_statistics("LHipRoll - stand", d_stand_LHipRoll_stats)

    d_walk_LHipRoll_stats = do_statistics(tr_walk[0], tr_walk[6])
    print_statistics("LHipRoll - walk", d_walk_LHipRoll_stats)

    # RKneePitch
    d_stand_RKneePitch_stats = do_statistics(tr_stand[0], tr_stand[7])
    print_statistics("RKneePitch - stand", d_stand_RKneePitch_stats)

    d_walk_RKneePitch_stats = do_statistics(tr_walk[0], tr_walk[7])
    print_statistics("RKneePitch - walk", d_walk_RKneePitch_stats)

    # LKneePitch
    d_stand_LKneePitch_stats = do_statistics(tr_stand[0], tr_stand[8])
    print_statistics("LKneePitch - stand", d_stand_LKneePitch_stats)

    d_walk_LKneePitch_stats = do_statistics(tr_walk[0], tr_walk[8])
    print_statistics("LKneePitch - walk", d_walk_LKneePitch_stats)

    # RAnklePitch
    d_stand_RAnklePitch_stats = do_statistics(tr_stand[0], tr_stand[9])
    print_statistics("RAnklePitch - stand", d_stand_RAnklePitch_stats)

    d_walk_RAnklePitch_stats = do_statistics(tr_walk[0], tr_walk[9])
    print_statistics("RAnklePitch - walk", d_walk_RAnklePitch_stats)
   
    # LAnklePitch
    d_stand_LAnklePitch_stats = do_statistics(tr_stand[0], tr_stand[10])
    print_statistics("LAnklePitch - stand", d_stand_LAnklePitch_stats)

    d_walk_LAnklePitch_stats = do_statistics(tr_walk[0], tr_walk[10])
    print_statistics("LAnklePitch - walk", d_walk_LAnklePitch_stats)
    
    # RAnkleRoll
    d_stand_RAnkleRoll_stats = do_statistics(tr_stand[0], tr_stand[11])
    print_statistics("RAnkleRoll - stand", d_stand_RAnkleRoll_stats)

    d_walk_RAnkleRoll_stats = do_statistics(tr_walk[0], tr_walk[11])
    print_statistics("RAnkleRoll - walk", d_walk_RAnkleRoll_stats)

    # LAnkleRoll
    d_stand_LAnkleRoll_stats = do_statistics(tr_stand[0], tr_stand[12])
    print_statistics("LAnkleRoll - stand", d_stand_LAnkleRoll_stats)

    d_walk_LAnkleRoll_stats = do_statistics(tr_walk[0], tr_walk[12])
    print_statistics("LAnkleRoll - walk", d_walk_LAnkleRoll_stats)

    stand2 = [x for x in a if x[13] == 4]
    walk2 = [x for x in a if x[13] == 5]

    d_stand = zip(*stand2)
    d_walk = zip(*walk2)
  
    pyplot.plot(d_stand[0], d_stand[7], ".", label="RKneePitch-stand")
    pyplot.plot(d_walk[0], d_walk[7], ".", label="RKneePitch-walk")

    pyplot.plot(d_stand[0], d_stand[8], ".", label="LKneePitch-stand")
    pyplot.plot(d_walk[0], d_walk[8], ".", label="LKneePitch-walk")

    pyplot.plot(d_stand[0], d_stand[9], ".", label="RAnklePitch-stand")
    pyplot.plot(d_walk[0], d_walk[9], ".", label="RAnklePitch-walk")
    
    pyplot.plot(d_stand[0], d_stand[10], ".", label="LAnklePitch-stand")
    pyplot.plot(d_walk[0], d_walk[10], ".", label="LAnklePitch-walk")

    # bb = zip(*data)
    # pyplot.plot(bb[0], bb[8], label="temperatur right")
    # pyplot.plot(bb[0], bb[7], label="temperatur left")
    # pyplot.plot(bb[0], bb[10], label="game")
    # pyplot.plot(bb[0], bb[5], label="motion")
    
    pyplot.legend(loc='upper left')
    pyplot.grid()
    pyplot.show()

log = init()
run(log)