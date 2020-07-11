from argparse import ArgumentParser
from naoth.log import transform_log_to_hdf5, get_representations
from naoth.datasets import motion

if __name__ == '__main__':
    parser = ArgumentParser(
        description='converts logfile to flat format')
    parser.add_argument("-i", "--input", help='logfile path')

    args = parser.parse_args()
    logfile_name = args.input if args.input else motion.load_data('cognition')

    # get all representations that are available in the logfiles
    representations = get_representations(logfile_name)

    # transform the log to flat format
    transformed_log = transform_log_to_hdf5(logfile_name, list(representations))
