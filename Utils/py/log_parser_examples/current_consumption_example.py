"""
    TODO make it more useful for use in other scripts:
    - make it possible to select specific joints
    - make it possible to select motion types / transition between motion types
    - add plots
"""
from argparse import ArgumentParser

import numpy as np
from naoth.log import BehaviorParser
from naoth.log import Reader as LogReader
from naoth.datasets import motion
from naoth.constants import get_joint_id


def frame_filter(behavior_parser, frame):
    try:
        behavior_frame = behavior_parser.parse(frame['BehaviorStateSparse'])

        return [frame["FrameInfo"].time / 1000.0,
                frame["BodyStatus"].currentSum[get_joint_id("RHipYawPitch")],
                frame["BodyStatus"].currentSum[get_joint_id("LHipYawPitch")],
                frame["BodyStatus"].currentSum[get_joint_id("RHipPitch")],
                frame["BodyStatus"].currentSum[get_joint_id("LHipPitch")],
                frame["BodyStatus"].currentSum[get_joint_id("RHipRoll")],
                frame["BodyStatus"].currentSum[get_joint_id("LHipRoll")],
                frame["BodyStatus"].currentSum[get_joint_id("RKneePitch")],
                frame["BodyStatus"].currentSum[get_joint_id("LKneePitch")],
                frame["BodyStatus"].currentSum[get_joint_id("RAnklePitch")],
                frame["BodyStatus"].currentSum[get_joint_id("LAnklePitch")],
                frame["BodyStatus"].currentSum[get_joint_id("RAnkleRoll")],
                frame["BodyStatus"].currentSum[get_joint_id("LAnkleRoll")],
                behavior_frame.input_symbols['executed_motion.type'].value
                ]

    except KeyError:
        raise StopIteration


def analyze_log(logfile):
    frame_list = list()

    with LogReader(logfile) as log:
        behavior_parser = BehaviorParser()
        for frame in log.read():
            if 'BehaviorStateComplete' in frame:
                behavior_parser.initialize(frame['BehaviorStateComplete'])
            frame_list.append(frame_filter(behavior_parser, frame))

    # make an numpy array
    data = np.array(frame_list)

    new_data = np.zeros((data.shape[0] - 1, data.shape[1]))

    for idx in range(data.shape[0] - 1):
        # calculate the frame time since last frame
        time_diff = data[idx + 1, 0] - data[idx, 0]
        # calculate the difference between the current sums
        current_difference = data[idx + 1, 1:13] - data[idx, 1:13]

        new_data[idx, 0] = time_diff
        new_data[idx, 1:13] = current_difference
        new_data[idx, 13] = data[idx, 13]  # leave executed motion type as it is

    # extract the frames where the robot was standing or walking
    stand = np.array([frame for frame in new_data if int(frame[13]) == 4])
    walk = np.array([frame for frame in new_data if int(frame[13]) == 5])

    # make some simple statistics on it
    if list(stand):
        time_stand = sum(stand[:, 0])
        consumption_r_hip_yaw_pitch_stand = sum(stand[:, 1])
        print("time standing: {} seconds with {} current used at RHipYawPitch"
              .format(time_stand, consumption_r_hip_yaw_pitch_stand))
    if list(walk):
        time_walk = sum(walk[:, 0])
        consumption_r_hip_yaw_pitch_walk = sum(walk[:, 1])
        print("time walking : {} seconds with {} current used at RHipYawPitch"
              .format(time_walk, consumption_r_hip_yaw_pitch_walk))


if __name__ == '__main__':
    parser = ArgumentParser(
        description='script to generate some energy statistics from logfile')
    parser.add_argument("-i", "--input", help='logfile, containing the BodyStatus representation and behavior')

    args = parser.parse_args()
    logfile_name = args.input if args.input else motion.load_data('cognition')
    analyze_log(logfile_name)
