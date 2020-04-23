"""
    TODO make it more useful to other scripts:
    - make it possible to calculate the offset between sensor and motor joint data
    - make it possible to calculate and show the data for specific joints

    For what kind of experiments do we actually need to look at sensor vs motor joint data?
"""
from argparse import ArgumentParser

from matplotlib import pyplot as plt
from naoth.log import Parser
from naoth.log import Reader as LogReader
from naoth.log import get_demo_logfiles

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
    return [frame["FrameInfo"].time / 1000.0,
            frame["MotionRequest"],
            frame["MotorJointData"],
            frame["SensorJointData"]
            ]


def analyze_log(logfile):
    behavior_parser = Parser()
    behavior_parser.register("MotorJointData", "JointData")

    log = LogReader(logfile, behavior_parser)

    frame_list = list()
    for frame in log:
        a = frame_filter(frame)
        frame_list.append(a)

    sensor_joint_pos_r_knee_pitch = list()
    motor_joint_pos_r_knee_pitch = list()

    for frame in frame_list:
        motor_joint_pos_r_knee_pitch.append(frame[2].position[16])
        sensor_joint_pos_r_knee_pitch.append(frame[3].jointData.position[16])

    fig, axs = plt.subplots(2)
    fig.suptitle('RKneePitch Position over time')
    axs[0].plot(sensor_joint_pos_r_knee_pitch)
    axs[0].set_title("Sensorjoint Data")
    axs[1].plot(motor_joint_pos_r_knee_pitch)
    axs[1].set_title("Motorjoint Data")
    plt.show()


if __name__ == '__main__':
    base_url = "https://www2.informatik.hu-berlin.de/~naoth/ressources/log/"
    logfile_list = ["walk_on_floor_motion.log"]
    get_demo_logfiles(base_url, logfile_list)

    parser = ArgumentParser(
        description='script for plotting the sensor and motor positions of a joint')
    parser.add_argument("-i", "--input", help='logfile, containing sensor and motor joint data',
                        default="logs/walk_on_floor_motion.log")

    args = parser.parse_args()

    analyze_log(args.input)
