"""
    combines image.logs and game.log files such that the resulting log contains images, frameinfo and camera matrix data
"""
import os
from argparse import ArgumentParser

from naoth.log import Reader as LogReader
from naoth.datasets import images
from naoth.pb.Framework_Representations_pb2 import Image


def create_image_log_dict(image_log, first_image_is_top):
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
        # assumes the first image is a bottom image
        # NOTE: this was changed in 2023, for older image logs this might need adjustment.
        is_camera_top = first_image_is_top
        while True:
        
            # read the frame number
            frame = f.read(4)
            if len(frame) != 4:
                break
                
            frame_number = int.from_bytes(frame, byteorder='little')

            # read the position of the image data block
            offset = f.tell()
            # skip the image data block
            f.seek(offset + image_data_size)

            # handle the case of incomplete image at the end of the logfile
            if f.tell() >= file_size:
                print("Info: frame {} in {} incomplete, missing {} bytes. Stop."
                      .format(frame_number, image_log, f.tell() + 1 - file_size))
                print("Info: Last frame seems to be incomplete.")
                break

            if frame_number not in images_dict:
                images_dict[frame_number] = {}
            
            name = 'ImageTop' if is_camera_top else 'Image'
            images_dict[frame_number][name] = (offset, image_data_size)

            # next image is of the other cam
            is_camera_top = not is_camera_top

    return images_dict


if __name__ == "__main__":
    parser = ArgumentParser(description='script for combining game.log and images.log files')
    parser.add_argument("-g", "--glog", help='normal game.log')
    parser.add_argument("-a", "--glog_all", help='Keep all frames from the game log.', action='store_true')
    
    parser.add_argument("-i", "--ilog", help='log containing the images')
    parser.add_argument("-t", "--ilog_first_image_top", help='First image in the image log is from the top camera. Use it for older logfiles.', action='store_true')
    
    parser.add_argument("-o", "--olog", help='output log file', default="combined.log")
    
    args = parser.parse_args()

    image_log = args.ilog if args.ilog else images.load_data('image_log')
    game_log = args.glog if args.glog else images.load_data('game_log')

    print("Indexing image log (first image: {})".format("top" if args.ilog_first_image_top else "bottom"))
    image_log_index = create_image_log_dict(image_log, args.ilog_first_image_top)

    print('Writing new log to: "{}"...'.format(args.olog))
    with open(args.olog, 'wb') as output, open(image_log, 'rb') as image_log, LogReader(game_log) as reader:
        for frame in reader.read():
            # only write frames which have corresponding images
            if frame.number in image_log_index:
                
                # may contain 'ImageTop' and 'Image'
                for image_name, (offset, size) in image_log_index[frame.number].items():
                    # load image data
                    image_log.seek(offset)
                    image_data = image_log.read(size)

                    # add image from image.log
                    msg = Image()
                    msg.height = 480
                    msg.width = 640
                    msg.format = Image.YUV422
                    msg.data = image_data
                    
                    frame.add_field(image_name, msg)

                # write the modified frame to the new log
                output.write(bytes(frame))

                # HACK: Frames are indexed by the log reader. Remove the image of already processed frames to preserve memory.
                for image_name in image_log_index[frame.number]:
                    frame.remove(image_name)
                    
            elif args.glog_all:
                # write unmodified frame to the new log
                output.write(bytes(frame))
                