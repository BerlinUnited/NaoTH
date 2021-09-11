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
    return [frame.number, image_from_proto(image_bottom), image_from_proto(image_top), cm_bottom,
            cm_top]


def get_patches(frame):
    ball_candidates = frame["BallCandidates"]
    ball_candidates_top = frame["BallCandidatesTop"]
    # print len(ball_candidates.patches), len(ball_candidates_top.patches)
    return [ball_candidates_top]


def read_all_patches_from_log(fileName, type=0):
    # initialize the parser
    my_parser = Parser()
    # register the protobuf message name for the 'ImageTop'
    my_parser.register("ImageTop", "Image")
    my_parser.register("BallCandidatesTop", "BallCandidates")
    my_parser.register("CameraMatrixTop", "CameraMatrix")

    # get all the images from the logfile
    # images = map(getPatches, LogReader(fileName, my_parser))

    camera_index = []
    patches = []
    for frame in LogReader(fileName, my_parser):
        try:
            ball_candidates = frame["BallCandidates"]
        except:
            # this can happen at the end of the log where the frame is not fully recorded
            print("An exception occurred")
            continue

        if ball_candidates is None:
            continue
        #print(ball_candidates)
        for p in ball_candidates.patches:
            if p.type == type:
                data = numpy.fromstring(p.data, dtype=numpy.uint8)
                patches.append(data)
                camera_index.append([0])


        try:
            ball_candidates_top = frame["BallCandidatesTop"]
        except:
            # this can happen at the end of the log where the frame is not fully recorded
            print("An exception occurred")
            continue
        # TODO check if there is a difference here to the try block
        if ball_candidates_top is None:
            continue
        for p in ball_candidates_top.patches:
            if p.type == type:
                data = numpy.fromstring(p.data, dtype=numpy.uint8)
                patches.append(data)
                camera_index.append([1])

    return patches, camera_index


if __name__ == "__main__":
    fileName = parse_arguments(sys.argv[1:])
    print(fileName)

    patches = read_all_patches_from_log(fileName)
    print(len(patches))
