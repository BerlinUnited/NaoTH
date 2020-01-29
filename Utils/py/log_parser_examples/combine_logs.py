"""
    combines image.logs and game.log files such that the resulting log contains images, frameinfo and camera matrix data
"""
from naoth.LogReader import LogReader
from naoth.LogReader import Parser
import struct
from naoth.Framework_Representations_pb2 import *


def create_image_log_dict():
    image_log = "logs/images.log"
    # parse image log
    width = 640
    height = 480
    bytes = 2

    images_dict = dict()

    with open(image_log, 'rb') as f:
        while True:
            frame = f.read(4)
            frame_number = int.from_bytes(frame, byteorder='little')
            data = f.read(width * height * bytes)

            # handle the case of incomplete image at the end of the logfile
            if len(data) != width * height * bytes:
                print("read only {0} bytes, but {1} needed.".format(len(data), width * height * bytes))
                break

            images_dict[frame_number] = data

    return images_dict


def create_game_log_dict():
    game_log = "logs/game.log"
    game_log_dict = dict()
    # parse game.log
    myParser = Parser()
    myParser.register("ImageTop", "Image")
    myParser.register("CameraMatrixTop", "CameraMatrix")
    myParser.register("BallPerceptTop", "BallPercept")
    myParser.register("ScanLineEdgelPerceptTop", "ScanLineEdgelPercept")
    myParser.register("GoalPerceptTop", "GoalPercept")
    myParser.register("BallCandidatesTop", "BallCandidates")
    myParser.register("FieldPerceptTop", "FieldPercept")

    for frame in LogReader(game_log, myParser):
        frame_number = frame['FrameInfo'].frameNumber
        game_log_dict[frame_number] = frame

    return game_log_dict


def write_message(file, frame_number, name, msg):
    print(name)
    file.write(struct.pack('I', frame_number))
    file.write(name.encode('ascii'))
    file.write(b'\0')

    # serialize message
    data = msg.SerializeToString()
    size = len(data)
    file.write(struct.pack('I', size))
    file.write(data)


if __name__ == "__main__":
    print("parse image log")
    images_dict = create_image_log_dict()
    print("parse game log")
    game_log_dict = create_game_log_dict()

    print("write new log")
    out = open("./tmp3.log", 'wb')
    for k, v in images_dict.items():
        gamelog_frame = game_log_dict[k]

        for name in gamelog_frame.messages:
            msg = gamelog_frame[name]
            write_message(out, gamelog_frame.number, name, msg)

        # add image from image.log
        msg = Image()
        msg.height = 480
        msg.width = 640
        msg.format = Image.YUV422
        msg.data = v
        # TODO unterscheiden zwischen imagetop und image
        write_message(out, gamelog_frame.number, "ImageTop", msg)  # ImageTop
        write_message(out, gamelog_frame.number, "Image", msg)  # ImageTop
