"""
    combines image.logs and game.log files such that the resulting log contains images, frameinfo and camera matrix data
"""
import struct
from argparse import ArgumentParser
from pathlib import Path

from naoth.pb.Framework_Representations_pb2 import Image
from naoth.log import Reader as LogReader
from naoth.log import Parser
from pywget import wget


def get_demo_logfiles():
    base_url = "https://www2.informatik.hu-berlin.de/~naoth/ressources/log/demo_image/"

    logfile_list = ["images.log", "game.log"]
    # taken from /vol/repl261-vol4/naoth/logs/2019-11-21_Koeln/
    # 2019-11-21_16-20-00_Berlin United_vs_Nao_Devils_Dortmund_half1/game_logs/1_96_Nao0377_191122-0148

    print("Downloading Logfiles: {}".format(logfile_list))

    target_dir = Path("logs")
    Path.mkdir(target_dir, exist_ok=True)

    print(" Download from: {}".format(base_url))
    print(" Download to: {}".format(target_dir.resolve()))
    for logfile in logfile_list:
        if not Path(target_dir / logfile).is_file():
            print("Download: {}".format(logfile))
            wget.download(base_url + logfile, str(target_dir))
            print("Done.")

    print("Finished downloading")


def create_image_log_dict(image_log):
    """
        Return the a dictionary with frame numbers as key and frames as values.
    """
    # parse image log
    width = 640
    height = 480
    bytes_per_pixel = 2
    image_data_size = width * height * bytes_per_pixel

    images_dict = dict()

    with open(image_log, 'rb') as f:
        while True:
            frame = f.read(4)
            frame_number = int.from_bytes(frame, byteorder='little')
            data = f.read(image_data_size)

            # handle the case of incomplete image at the end of the logfile
            if len(data) != image_data_size:
                print("Info: Read from {} frame {}: only {} bytes, but {} expected. Stop.".format(
                    image_log, frame_number, len(data), image_data_size))
                print("Info: Last frame seems to be incomplete.")
                break

            images_dict[frame_number] = data

    return images_dict


def create_game_log_dict(game_log):
    """
        Returns the a dictionary with frame numbers as key and frames as values.
    """
    game_log_dict = dict()
    # parse game.log
    my_parser = Parser()
    my_parser.register("ImageTop", "Image")
    my_parser.register("CameraMatrixTop", "CameraMatrix")
    my_parser.register("BallPerceptTop", "BallPercept")
    my_parser.register("ScanLineEdgelPerceptTop", "ScanLineEdgelPercept")
    my_parser.register("GoalPerceptTop", "GoalPercept")
    my_parser.register("BallCandidatesTop", "BallCandidates")
    my_parser.register("FieldPerceptTop", "FieldPercept")

    for frame in LogReader(game_log, my_parser):
        game_log_dict[frame.number] = frame

    return game_log_dict


def write_message(file, frame_number, name, msg):
    file.write(struct.pack('I', frame_number))
    file.write(name.encode('ascii'))
    file.write(b'\0')

    # serialize message
    data = msg.SerializeToString()
    size = len(data)
    file.write(struct.pack('I', size))
    file.write(data)


def write_log(output_log, game_log, image_log, cam_bottom):
    output = open(output_log, 'wb')

    for k, v in image_log.items():
        gamelog_frame = game_log[k]

        for name in gamelog_frame.messages:
            msg = gamelog_frame[name]
            write_message(output, gamelog_frame.number, name, msg)

        # add image from image.log
        msg = Image()
        msg.height = 480
        msg.width = 640
        msg.format = Image.YUV422
        msg.data = v

        if cam_bottom:
            write_message(output, gamelog_frame.number, "Image", msg)  # ImageTop
        else:
            write_message(output, gamelog_frame.number, "ImageTop", msg)  # ImageTop

        # switch camera id
        cam_bottom = not cam_bottom


if __name__ == "__main__":
    get_demo_logfiles()

    parser = ArgumentParser(
        description='script for combining game.log and images.log files')
    parser.add_argument("-g", "--glog", help='normal game.log', default="logs/game.log")
    parser.add_argument("-i", "--ilog", help='log containing the images', default="logs/images.log")
    parser.add_argument("-o", "--olog", help='output log file', default="logs/combined.log")
    args = parser.parse_args()

    print("parse game log")
    parsed_game_log = create_game_log_dict(args.glog)

    print("parse image log")
    parsed_image_log = create_image_log_dict(args.ilog)

    print("write new log to: {}".format(args.olog))
    camera_bottom = False  # assumes the first image is a top image
    write_log(args.olog, parsed_game_log, parsed_image_log, camera_bottom)
