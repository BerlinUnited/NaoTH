"""
    TODO make it more useful for use in other scripts:
    - make it possible to select specific joints
    - make it possible to select motion types / transition between motion types
    - add plots
"""
from argparse import ArgumentParser
from pathlib import Path

import numpy as np
from naoth.log import BehaviorParser
from naoth.log import Reader as LogReader
from pywget import wget

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


def get_demo_logfiles():
    base_url = "https://www2.informatik.hu-berlin.de/~naoth/ressources/log/"

    logfile_list = ["walk_on_floor_cognition.log"]

    print("Downloading Logfiles: {}".format(logfile_list))

    target_dir = Path("logs")
    Path.mkdir(target_dir, exist_ok=True)

    print(" Download from: {}".format(base_url))
    print(" Download to: {}".format(target_dir.resolve()))
    for logfile in logfile_list:
        if not Path(target_dir / logfile).is_file():
            print("Download: {}".format(logfile))
            wget.download(base_url + logfile, str(target_dir))
            print("Done.")

    print("Finished downloading")


def frame_filter(idx, frame):
    """
    HACK The index of the log frame is given as argument because the BehaviorParser now does not work if the
    BehaviorStateComplete is in more than one frame. For manually recorded logs this can happen.
    """
    try:
        if "BehaviorStateComplete" in frame.messages and idx == 0:
            (m, o, s) = frame["BehaviorStateComplete"]
        else:
            (m, o, s) = frame["BehaviorStateSparse"]

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
                s.values["executed_motion.type"]
                ]

    except KeyError:
        raise StopIteration


def analyze_log(logfile):
    behavior_parser = BehaviorParser()
    log = LogReader(logfile, behavior_parser)

    frame_list = list()
    for idx, frame in enumerate(log):
        a = frame_filter(idx, frame)
        frame_list.append(a)

    # make an numpy array
    data = np.array(frame_list)

    # calculate the frame time since last frame
    new_data = np.zeros((data.shape[0] - 1, data.shape[1]))

    for idx in range(data.shape[0] - 1):
        time_diff = data[idx + 1, 0] - data[idx, 0]
        new_data[idx, :] = data[idx, :]
        new_data[idx, 0] = time_diff

    # extract the frames where the robot was standing or walking
    stand = np.array([frame for frame in new_data if int(frame[13]) == 4])
    walk = np.array([frame for frame in new_data if int(frame[13]) == 5])

    # make some simple statistics on it
    if list(stand):
        time_stand = sum(stand[:, 0])
        consumption_r_hip_yaw_pitch_stand = sum(stand[:, 1])
        print("time standing: {} seconds with {} current used at RHipYawPitch".format(time_stand,
                                                                                      consumption_r_hip_yaw_pitch_stand))
    if list(walk):
        time_walk = sum(walk[:, 0])
        consumption_r_hip_yaw_pitch_walk = sum(walk[:, 1])
        print("time walking : {} seconds with {} current used  at RHipYawPitch".format(time_walk,
                                                                                       consumption_r_hip_yaw_pitch_walk))


if __name__ == '__main__':
    get_demo_logfiles()

    parser = ArgumentParser(
        description='script to generate some energy statistics from logfile')
    parser.add_argument("-i", "--input", help='logfile, containing the images',
                        default="logs/walk_on_floor_cognition.log")

    args = parser.parse_args()

    analyze_log(args.input)
