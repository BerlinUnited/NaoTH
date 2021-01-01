"""
    TODO make it more useful to other scripts:
    - make it possible to calculate and show the data for specific joints and specific motion types
    - calculate the the temperature increase by motion type ->
        How much does executing a specific motion increase the temperature
"""
import sys
from argparse import ArgumentParser

from matplotlib import pyplot as plt
from naoth.log import BehaviorParser
from naoth.log import Reader as LogReader
from naoth.datasets import motion


def frame_filter(behavior_parser, frame):
    """
    Retrieves leg joint temperature data from a log file frame using the given behavior parser.

    @param behavior_parser: BehaviorParser instance initialized with a BehaviorStateComplete
    @param frame: Current log file frame
    @return current frame time, left leg joint temperature, right leg joint temperature, executed motion type
    """
    behavior_frame = behavior_parser.parse(frame['BehaviorStateSparse'])

    return [frame['FrameInfo'].time / 1000.0,
            behavior_frame.input_symbols['body.temperature.leg.left'],
            behavior_frame.input_symbols['body.temperature.leg.right'],
            behavior_frame.input_symbols['executed_motion.type']
            ]


def analyze_log(logfile):
    times = []
    left_leg_temps = []
    right_leg_temps = []
    motion_types = []

    behavior_parser = BehaviorParser()
    with LogReader(logfile) as reader:
        for frame in reader.read():
            # The behavior parser must be initialized with BehaviorStateComplete (should be in the first log file frame),
            # otherwise parsing will fail
            if 'BehaviorStateComplete' in frame:
                behavior_parser.initialize(frame['BehaviorStateComplete'])

            try:
                time, left_temp, right_temp, motion_type = frame_filter(behavior_parser, frame)

                times.append(time)
                left_leg_temps.append(left_temp)
                right_leg_temps.append(right_temp)
                # We are planning to just plot the name of the enum value
                motion_types.append(motion_type.name)
            except KeyError:
                print('Error - Missing fields in behavior frame.')
                sys.exit(-1)

    # Visualize the retrieved data
    fig, axs = plt.subplots(3, 1, constrained_layout=True)
    fig.suptitle('Temperature in leg joints over time\n')

    axs[0].plot(times, left_leg_temps)
    axs[0].set_title('Temperature left leg')

    axs[1].plot(times, right_leg_temps)
    axs[1].set_title('Temperature right leg')

    axs[2].plot(times, motion_types)
    axs[2].set_title('Executed motion type')

    plt.show()


if __name__ == '__main__':
    parser = ArgumentParser(description='script to generate some energy statistics from logfile')
    parser.add_argument("-i", "--input", help='logfile, containing the behavior')

    args = parser.parse_args()

    logfile = args.input if args.input else motion.load_data('cognition')

    analyze_log(logfile)
