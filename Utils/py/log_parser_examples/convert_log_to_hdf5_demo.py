from argparse import ArgumentParser
from naoth.log import transform_log_to_hdf5, get_representations, Parser, Reader
from naoth.datasets import motion

if __name__ == '__main__':
    parser = ArgumentParser(
        description='converts logfile to flat format')
    parser.add_argument("-i", "--input", help='logfile path')

    args = parser.parse_args()
    # get a logfile
    logfile_name = args.input if args.input else motion.load_data('cognition')

    # get all representations that are available in the logfiles
    representations = get_representations(logfile_name)

    # parse logfile option 1
    my_parser = Parser()
    log = Reader(logfile_name, my_parser)

    accel_data1 = list()
    for idx, frame in enumerate(log):
        x_accel = frame['AccelerometerData'].data.x
        accel_data1.append(x_accel)

    # parse logfile option 2: transform the log to flat format first
    transformed_log = transform_log_to_hdf5(logfile_name, list(representations))
    accel_data2 = transformed_log['AccelerometerData']['data']['x']

    # compare the output
    accel_data1.sort()
    accel_data2.sort()

    if accel_data1 == accel_data2:
        print("The lists are identical")
    else:
        print("The lists are not identical")
