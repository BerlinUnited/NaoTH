"""
    combines image.logs and game.log files such that the resulting log contains images, frameinfo and camera matrix data
"""
import os
from argparse import ArgumentParser

from naoth.log import Reader as LogReader
from naoth.datasets import images
from naoth.pb.Framework_Representations_pb2 import Image


def create_image_log_dict(image_log):
    """
    Return a dictionary with frame numbers as key and (offset, size, is_camera_bottom) tuples of image data as values.
    """
    # parse image log
    width = 640
    height = 480
    bytes_per_pixel = 2
    image_data_size = width * height * bytes_per_pixel

    file_size = os.path.getsize(image_log)

    images_dict = dict()

    with open(image_log, 'rb') as f:
        is_camera_bottom = False  # assumes the first image is a top image
        while True:
            frame = f.read(4)
            if len(frame) != 4:
                break
            frame_number = int.from_bytes(frame, byteorder='little')

            offset = f.tell()
            f.seek(offset + image_data_size)

            # handle the case of incomplete image at the end of the logfile
            if f.tell() >= file_size:
                print("Info: frame {} in {} incomplete, missing {} bytes. Stop."
                      .format(frame_number, image_log, f.tell() + 1 - file_size))
                print("Info: Last frame seems to be incomplete.")
                break

            images_dict[frame_number] = (offset, image_data_size, is_camera_bottom)
            # next image is of the other cam
            is_camera_bottom = not is_camera_bottom

    return images_dict


if __name__ == "__main__":
    parser = ArgumentParser(description='script for combining game.log and images.log files')
    parser.add_argument("-g", "--glog", help='normal game.log')
    parser.add_argument("-i", "--ilog", help='log containing the images')
    parser.add_argument("-o", "--olog", help='output log file', default="combined.log")
    args = parser.parse_args()

    image_log = args.ilog if args.ilog else images.load_data('image_log')
    game_log = args.glog if args.glog else images.load_data('game_log')

    print("Indexing image log...")
    image_log_index = create_image_log_dict(image_log)

    print('Writing new log to: "{}"...'.format(args.olog))
    with open(args.olog, 'wb') as output, open(image_log, 'rb') as image_log, LogReader(game_log) as reader:
        for frame in reader.read():
            # only write frames which have corresponding images
            if frame.number in image_log_index:
                # load image data
                offset, size, camera_bottom = image_log_index[frame.number]
                image_name = 'Image' if camera_bottom else 'ImageTop'
                image_log.seek(offset)
                image_data = image_log.read(size)

                # add image from image.log
                msg = Image()
                msg.height = 480
                msg.width = 640
                msg.format = Image.YUV422
                msg.data = image_data

                frame.add_field(image_name, msg)

                output.write(bytes(frame))

                # Frames are indexed by the log reader. Remove the image of already processed frames to preserve memory
                frame.remove(image_name)
