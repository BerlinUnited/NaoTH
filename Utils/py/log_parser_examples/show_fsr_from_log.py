import argparse
from collections import defaultdict

import matplotlib.pyplot as plt
import numpy as np
from naoth.log import Parser
from naoth.log import Reader as LogReader
from naoth.datasets import fsr


def get_fsr_data(frame):
    # we are only interested in top images
    fsr_data = frame["FSRData"]
    fsr_data_left = np.asarray(fsr_data.data[0:4])
    fsr_data_right = np.asarray(fsr_data.data[4:8])

    return [frame.number, fsr_data_left, fsr_data_right]


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='example of how to parse fsr data from log')
    parser.add_argument("-i", "--input", help='path to logfile')
    args = parser.parse_args()

    if args.input:
        log = [args.input]
    else:
        log = fsr.load_data()

    # initialize the log parser
    myParser = Parser()

    # get all of the fsr data from the logfile
    with LogReader(log[0], parser=myParser) as reader:
        data = map(get_fsr_data, reader.read())

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

        figure_left, axes_left = plt.subplots(4, constrained_layout=True)
        figure_left.suptitle('FSR Left Foot')
        axes_left[0].plot(frame_numbers, left_data["FrontLeft"])
        axes_left[1].plot(frame_numbers, left_data["FrontRight"])
        axes_left[2].plot(frame_numbers, left_data["RearLeft"])
        axes_left[3].plot(frame_numbers, left_data["RearRight"])

        figure_right, axes_right = plt.subplots(4, constrained_layout=True)
        figure_right.suptitle('FSR Right Foot')
        axes_right[0].plot(frame_numbers, right_data["FrontLeft"])
        axes_right[1].plot(frame_numbers, right_data["FrontRight"])
        axes_right[2].plot(frame_numbers, right_data["RearLeft"])
        axes_right[3].plot(frame_numbers, right_data["RearRight"])

        plt.show()
