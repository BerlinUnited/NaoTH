from naoth.log import Reader as LogReader
from naoth.log import Parser

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

# used for easier indexing of the data matrix
TimeIdx = 0

CurrentIdx = {
    "RHipYawPitch": 1,
    "LHipYawPitch": 2,
    "RHipPitch": 3,
    "LHipPitch": 4,
    "RHipRoll": 5,
    "LHipRoll": 6,
    "RKneePitch": 7,
    "LKneePitch": 8,
    "RAnklePitch": 9,
    "LAnklePitch": 10,
    "RAnkleRoll": 11,
    "LAnkleRoll": 12
}

TemperatureIdx = {
    "RHipYawPitch": 13,
    "LHipYawPitch": 14,
    "RHipPitch": 15,
    "LHipPitch": 16,
    "RHipRoll": 17,
    "LHipRoll": 18,
    "RKneePitch": 19,
    "LKneePitch": 20,
    "RAnklePitch": 21,
    "LAnklePitch": 22,
    "RAnkleRoll": 23,
    "LAnkleRoll": 24
}

CurrentMotionIdx = 25


def frame_filter(frame):
    try:
        return [frame["FrameInfo"].time / 1000.0,
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
        print("error while applying the frame_filter")
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
    # print("var  (time weighted): {}".format(stats[1]))
    print("std  (time weighted): {}".format(stats[2]))
    # print("mean: {}".format(stats[3]))
    # print("var : {}".format(stats[4]))
    # print("std : {}".format(stats[5]))


def plot(data, joints, rows, cols):
    fig, ax = pyplot.subplots(rows, cols)
    ax = [axis for vertical_sub_plot in ax for axis in vertical_sub_plot]
    for (ax1, joint) in zip(ax, joints):
        ax1.plot(data[0][TimeIdx], data[0][CurrentIdx[joint]], "b")
        ax1.plot(data[1][TimeIdx], data[1][CurrentIdx[joint]], "b--")
        ax1.set_xlabel('time (s)')
        ax1.set_ylabel('cumulative current (A)')
        for tl in ax1.get_yticklabels():
            tl.set_color('b')

        ax2 = ax1.twinx()
        ax2.plot(data[0][TimeIdx], data[0][TemperatureIdx[joint]], "r", label=joint + " standard")
        ax2.plot(data[1][TimeIdx], data[1][TemperatureIdx[joint]], "r--", label=joint + " simple stiff joint control")
        ax2.set_ylabel('temperature (Celsius)')
        for tl in ax2.get_yticklabels():
            tl.set_color('r')

        pyplot.legend(loc='upper left')
        pyplot.grid()
    pyplot.show()


def init():
    parser = Parser()
    # "/home/steffen/NaoTH/Logs/EuropeanOpen16/2016-03-30-test-naodevils/half1/160330-1952-Nao6022"
    file_name = "logs/game_eu.log"
    log0 = LogReader(file_name, parser)

    file_name = "logs/game_eu.log"
    log1 = LogReader(file_name, parser)

    return [log0, log1]


def run(log):
    logs = log

    print("representations check --- start")

    print("contained representations")
    i = 0
    for log in logs:
        i = i + 1
        print("log no.{}:".format(i))
        print(log.names)

    # check if all required representations are contained in log
    required_reps = ['BodyStatus', 'FrameInfo', 'MotionStatus', 'OffsetJointData', 'SensorJointData']

    missing_rep = False
    for rep in required_reps:
        i = 0
        for log in logs:
            i = i + 1
            if rep not in log.names:
                print(
                    "ERROR: representation " + rep + " is not contained in log no.{} this log file but required by this script".format(
                        i))
                missing_rep = True

    if missing_rep:
        return

    print("representations check --- done")

    print("data filtering --- start")

    # apply the filter
    a = map(frame_filter, logs[0])
    b = map(frame_filter, logs[1])

    print("data filtering --- done")

    data = [np.array(zip(*a)), np.array(zip(*b))]

    print("bias removing --- start")

    # remove bias from current
    for d in data:

        for idx in iter(CurrentIdx.values()):
            d[idx] = d[idx] - d[idx][0]

    # remove bias from time
    for d in data:
        d[TimeIdx] = d[TimeIdx] - d[TimeIdx][0]

    print("bias removing --- done")

    plot(data, ("RHipPitch", "LHipPitch", "RKneePitch", "LKneePitch", "RAnklePitch", "LAnklePitch"), 3, 2)
    # for k in CurrentIdx:
    #    plot(data,k)


if __name__ == '__main__':
    logs = init()
    run(logs)
