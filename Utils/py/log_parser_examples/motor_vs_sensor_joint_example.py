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
from naoth.datasets import motion
from naoth.constants import get_joint_id


# TODO use joint definitions from naoth.log._constants
def frame_filter(frame):
    return [frame["FrameInfo"].time / 1000.0,
            frame["MotionRequest"],
            frame["MotorJointData"],
            frame["SensorJointData"]
            ]


def analyze_log(logfile):
    parser = Parser()

    with LogReader(logfile, parser) as log:
        frame_list = list()
        for frame in log.read():
            a = frame_filter(frame)
            frame_list.append(a)

        sensor_joint_pos_r_knee_pitch = list()
        motor_joint_pos_r_knee_pitch = list()

        for frame in frame_list:
            motor_joint_pos_r_knee_pitch.append(frame[2].position[get_joint_id("RKneePitch")])
            sensor_joint_pos_r_knee_pitch.append(frame[3].jointData.position[get_joint_id("RKneePitch")])

        fig, axs = plt.subplots(2, constrained_layout=True)
        fig.suptitle('RKneePitch Position over time')
        axs[0].plot(sensor_joint_pos_r_knee_pitch)
        axs[0].set_title("Sensorjoint Data")
        axs[1].plot(motor_joint_pos_r_knee_pitch)
        axs[1].set_title("Motorjoint Data")
        plt.show()


if __name__ == '__main__':
    parser = ArgumentParser(
        description='script for plotting the sensor and motor positions of a joint')
    parser.add_argument("-i", "--input", help='logfile, containing sensor and motor joint data')
    args = parser.parse_args()

    logfile_name = args.input if args.input else motion.load_data('motion')

    analyze_log(logfile_name)
