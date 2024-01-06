import sys
import getopt

from naoth.log import Reader as LogReader
from naoth.log import Parser
import numpy
from PIL import Image


def parse_arguments(argv):
    input_file = ''

    try:
        opts, args = getopt.getopt(argv, "hi:", ["ifile="])
    except getopt.GetoptError:
        print('patchReader.py -i <input file>')
        sys.exit(2)
    if not opts:
        print('python patchReader.py -i <logfile>')
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print('patchReader.py -i <input file>')
            sys.exit()
        elif opt in ("-i", "--ifile"):
            input_file = arg

    return input_file


def image_from_proto(message):
    # read each channel of yuv422 separately
    yuv422 = numpy.fromstring(message.data, dtype=numpy.uint8)
    y = yuv422[0::2]
    u = yuv422[1::4]
    v = yuv422[3::4]

    # convert from yuv422 to yuv888
    yuv888 = numpy.zeros(message.height * message.width * 3, dtype=numpy.uint8)
    yuv888[0::3] = y
    yuv888[1::6] = u
    yuv888[2::6] = v
    yuv888[4::6] = u
    yuv888[5::6] = v

    yuv888 = yuv888.reshape(message.height, message.width, 3)

    # convert the image to rgb and save it
    img = Image.fromstring('YCbCr', (message.width, message.height), yuv888.tostring())
    return img


def get_images(frame):
    # we are only interested in top images
    image_top = frame["ImageTop"]
    image_bottom = frame["Image"]
    cm_bottom = frame["CameraMatrix"]
    cm_top = frame["CameraMatrixTop"]

    return [
        frame.number, 
        image_from_proto(image_bottom), 
        image_from_proto(image_top), 
        cm_bottom,
        cm_top
    ]


def get_ball_candidates(frame):
    try:
        ball_candidates_top = frame["BallCandidatesTop"]
    except:
        ball_candidates_top = None

    try:
        ball_candidates_bottom = frame["BallCandidates"]
    except:
        ball_candidates_bottom = None

    return (ball_candidates_top, ball_candidates_bottom)


def read_all_patches_from_log(fileName, type=0):
    my_parser = Parser()

    # register the protobuf message names which are not defined in the log file by default
    my_parser.register("ImageTop", "Image")
    my_parser.register("BallCandidatesTop", "BallCandidates")
    my_parser.register("CameraMatrixTop", "CameraMatrix")

    camera_indices = []
    patches = []

    for frame in LogReader(fileName, my_parser):
        ball_candidates_top, ball_candidates_bottom = get_ball_candidates(frame)

        if ball_candidates_top is not None and ball_candidates_top.patches:
            for p in ball_candidates_top.patches:
                data = numpy.frombuffer(p.data, dtype=numpy.uint8)
                patches.append(data)
                camera_indices.append([1])

        if ball_candidates_bottom is not None and ball_candidates_bottom.patches:
            for p in ball_candidates_bottom.patches:
                data = numpy.frombuffer(p.data, dtype=numpy.uint8)
                patches.append(data)
                camera_indices.append([0])

    return patches, camera_indices


if __name__ == "__main__":
    fileName = parse_arguments(sys.argv[1:])
    print(fileName)

    patches, camera_index = read_all_patches_from_log(fileName)
    print(len(patches))