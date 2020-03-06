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

        return [frame["FrameInfo"].time / 1000.0,
                m["body.temperature.leg.left"],
                m["body.temperature.leg.right"],
                m["executed_motion.type"]
                ]

    except KeyError:
        raise StopIteration


def do_statistics(time, data):
    time_total = sum(time)

    d_data = map(lambda c, t: c / t, data, time)

    weighted_mean = np.average(d_data, axis=0, weights=time / time_total)
    weighted_var = np.average((d_data - weighted_mean) ** 2, axis=0, weights=time / time_total)
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
    print("var  (time weighted): {}".format(stats[1]))
    print("std  (time weighted): {}".format(stats[2]))
    print("mean: {}".format(stats[3]))
    print("var : {}".format(stats[4]))
    print("std : {}".format(stats[5]))


def init():
    # file_name = "/home/steffen/NaoTH/Logs/MesseLeipzig/lm15-naodevils-2/"
    # file_name = file_name + "151004-1919-Nao6029"
    file_name = "/home/steffen/NaoTH/Logs/EuropeanOpen16/2016-03-30-test-naodevils/half1/"
    file_name = file_name + "160330-1952-Nao6022"
    file_name = file_name + "/game.log"
    parser = BehaviorParser.BehaviorParser()
    log = BehaviorParser.LogReader(file_name, parser)

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
    vlog = filter(lambda f: "BehaviorStateComplete" in f.messages or "BehaviorStateSparse" in f.messages, log)

    # apply the filter
    a = map(frame_filter, vlog)

    # make an numpy array
    data = np.array(a)

    size = data.shape

    data2 = np.empty([0, 4])

    current = 0
    for x in range(1, size[0]):
        if data[current, 3] == data[x, 3]:
            continue
        elif data[current, 3] == 4 or data[current, 3] == 5:
            temp = data[x, :] - data[current, :]
            temp[3] = data[current, 3]
            current = x
            data2 = np.vstack((data2, temp))
        else:
            current = x

    stand = [x for x in data2 if x[3] == 4]
    walk = [x for x in data2 if x[3] == 5]

    tr_stand = zip(*stand)
    tr_walk = zip(*walk)

    time_stand = sum(tr_stand[0])
    time_walk = sum(tr_walk[0])

    print("------------------")
    print("time standing: {}".format(time_stand))
    print("time walking : {}".format(time_walk))
    print("time total   : {}".format(time_stand + time_walk))

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
    # pyplot.plot(bb[0], bb[10], label="game")
    # pyplot.plot(bb[0], bb[5], label="motion")

    pyplot.legend(loc='upper left')
    pyplot.grid()
    pyplot.show()
