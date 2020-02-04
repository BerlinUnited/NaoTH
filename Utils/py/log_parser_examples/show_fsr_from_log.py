import argparse
from pywget import wget
import numpy as np
import matplotlib.pyplot as plt
from collections import defaultdict
from pathlib import Path

from naoth.log import Reader as LogReader
from naoth.log import Parser


def get_all_fsr_demo_logfiles():
    base_url = "https://www2.informatik.hu-berlin.de/~naoth/ressources/log/demo_fsr/"
    logfile_list = ["fallen_motion.log", "sit_motion.log", "stand_motion.log", "walk_motion.log"]

    target_dir = Path("logs")
    Path.mkdir(target_dir, exist_ok=True)

    for logfile in logfile_list:
        if not Path(target_dir / logfile).is_file():
            wget.download(base_url+logfile, target_dir)


def get_fsr_data(frame):
    # we are only interested in top images
    fsr_data = frame["FSRData"]
    fsr_data_left = np.asarray(fsr_data.data[0:4])
    fsr_data_right = np.asarray(fsr_data.data[4:8])

    return [frame.number, fsr_data_left, fsr_data_right]


if __name__ == "__main__":
    get_all_fsr_demo_logfiles()

    parser = argparse.ArgumentParser(
        description='example of how to parse fsr data from log')
    parser.add_argument("-i", "--input", help='path to logfile', default="logs/fallen_motion.log")

    args = parser.parse_args()

    # initialize the log parser
    myParser = Parser()
    # register the protobuf message name for the 'ImageTop'
    myParser.register("FSRData", "FSRData")

    # get all of the fsr data from the logfile
    data = map(get_fsr_data, LogReader("logs/stand_motion.log", myParser))

    frame_numbers = []
    left_data = defaultdict(list)
    right_data = defaultdict(list)

    for frame_num, fsr_left, fsr_right in data:
        frame_numbers.append(frame_num)
        left_data["FrontLeft"].append(fsr_left[0])
        left_data["FrontRight"].append(fsr_left[1])
        left_data["RearLeft"].append(fsr_left[2])
        left_data["RearRight"].append(fsr_left[3])

        right_data["FrontLeft"].append(fsr_right[0])
        right_data["FrontRight"].append(fsr_right[1])
        right_data["RearLeft"].append(fsr_right[2])
        right_data["RearRight"].append(fsr_right[3])

    figure_left, axes_left = plt.subplots(4)
    figure_left.suptitle('FSR Left Foot')
    axes_left[0].plot(frame_numbers, left_data["FrontLeft"])
    axes_left[1].plot(frame_numbers, left_data["FrontRight"])
    axes_left[2].plot(frame_numbers, left_data["RearLeft"])
    axes_left[3].plot(frame_numbers, left_data["RearRight"])

    figure_right, axes_right = plt.subplots(4)
    figure_right.suptitle('FSR Right Foot')
    axes_right[0].plot(frame_numbers, right_data["FrontLeft"])
    axes_right[1].plot(frame_numbers, right_data["FrontRight"])
    axes_right[2].plot(frame_numbers, right_data["RearLeft"])
    axes_right[3].plot(frame_numbers, right_data["RearRight"])

    plt.show()
