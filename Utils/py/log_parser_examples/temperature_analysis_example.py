"""
    TODO make it more useful to other scripts:
    - make it possible to calculate and show the data for specific joints and specific motion typesd
    - calculate the the temperature increase by motion type ->
        How much does executing a specific motion increase the temperature
"""
from argparse import ArgumentParser
from pathlib import Path

import numpy as np
from matplotlib import pyplot as plt
from naoth.log import BehaviorParser
from naoth.log import Reader as LogReader
from pywget import wget


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
                s.values["body.temperature.leg.left"],
                s.values["body.temperature.leg.right"],
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
    time = data[:, 0]
    temp_left = data[:, 1]
    temp_right = data[:, 2]
    motion_type = data[:, 3]

    fig, axs = plt.subplots(3)
    fig.suptitle('Temperature in leg joints over time')
    axs[0].plot(time, temp_left, label="temperature left")
    axs[1].plot(time, temp_right, label="temperature right")
    axs[2].plot(time, motion_type, label="motion type")
    plt.show()


if __name__ == '__main__':
    get_demo_logfiles()

    parser = ArgumentParser(
        description='script to generate some energy statistics from logfile')
    parser.add_argument("-i", "--input", help='logfile, containing the behavior',
                        default="logs/walk_on_floor_cognition.log")

    args = parser.parse_args()

    analyze_log(args.input)
