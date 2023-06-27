
import sys
from naoth.log import Reader as LogReader

if __name__ == "__main__":

    if len(sys.argv) != 2:
        print("Need one path as argument:")
        print("  python inspect_log.py path/to/my.log")
        quit()

    filename = sys.argv[1]
    print("inspect logfile {}".format(filename))

    with LogReader(filename) as reader:
        
        # read all frames one by one
        for frame in reader.read():
            print(frame.number)
            # list contained fields
            for name in frame.get_names():
                print('    {}'.format(name))
