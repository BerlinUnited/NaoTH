#!/usr/bin/python
import sys
import getopt
from naoth.LogReader import LogReader
from naoth.LogReader import Parser


def parse_arguments(argv):
    input_file = ''
    try:
        opts, args = getopt.getopt(argv, "hi:", ["ifile="])
    except getopt.GetoptError:
        print('python exportEdgels.py -i <logfile>')
        sys.exit(2)
    if not opts:
        print('python exportEdgels.py -i <logfile>')
        sys.exit(2)
    for opt, arg in opts:
        print("opts: ", opts)
        if opt == '-h':
            print('python exportEdgels.py -i <logfile>')
            sys.exit()
        elif opt in ("-i", "--ifile"):
            input_file = arg

    return input_file


def get_camera_matrix(frame):
    cm_bottom = frame["CameraMatrix"]
    cm_top = frame["CameraMatrixTop"]
    return [frame.number, cm_bottom, cm_top]


def get_edgels(frame):
     edgel_percepts = frame["ScanLineEdgelPercept"]
     return [frame.number, edgel_percepts]


if __name__ == "__main__":
    logFilePath = parse_arguments(sys.argv[1:])

    myParser = Parser()
    myParser.register("CameraMatrixTop", "CameraMatrix")

    for msg in LogReader(logFilePath, myParser, get_edgels):
        print(msg)
