"""
    this script exports images from the plain image log to png files and saves the guessed camera id in the
    png info header
"""
import multiprocessing as mp
import os
import sys
from argparse import ArgumentParser
from pathlib import Path

import numpy
from PIL import Image
from PIL import PngImagePlugin
from naoth.datasets import images


def image_from_data(image_data, width, height):
    # read each channel of yuv422 separately
    yuv422 = numpy.fromstring(image_data, dtype=numpy.uint8)
    y = yuv422[0::2]
    u = yuv422[1::4]
    v = yuv422[3::4]

    # convert from yuv422 to yuv888
    yuv888 = numpy.zeros(height * width * 3, dtype=numpy.uint8)
    yuv888[0::3] = y
    yuv888[1::6] = u
    yuv888[2::6] = v
    yuv888[4::6] = u
    yuv888[5::6] = v

    yuv888 = yuv888.reshape((height, width, 3))

    # convert the image to rgb and save it
    img = Image.frombytes('YCbCr', (width, height), yuv888.tostring())
    return img


def save_data_to_png(image_data, width, height, output_path, cam_flag):
    if cam_flag:
        cam_id = 1
    else:
        cam_id = 0
    meta = PngImagePlugin.PngInfo()
    meta.add_text("CameraID", str(cam_id))

    img = image_from_data(image_data, width, height)
    img = img.convert('RGB')
    img.save(output_path, pnginfo=meta)
    return True


def parse_image_log(input_path, width, height, image_bytes, cam_bottom):
    # create output folder
    logfile_name = Path(input_path).stem
    output_folder = Path(logfile_name)
    output_folder.mkdir(exist_ok=True)

    # read all data
    image_data = list()
    with open(input_path, 'rb') as f:
        while True:
            frame = f.read(4)
            frame_number = int.from_bytes(frame, byteorder='little')

            data = f.read(width * height * image_bytes)

            # handle the case of incomplete image at the end of the logfile
            if len(data) != width * height * image_bytes:
                print("read only {0} bytes, but {1} needed.".format(len(data), width * height * image_bytes))
                break

            img_path = os.path.join(str(output_folder), "{0}.png".format(frame_number))
            image_data += [(data, args.width, args.height, img_path, cam_bottom)]
            # switch camera id
            cam_bottom = not cam_bottom

    return image_data


if __name__ == "__main__":
    parser = ArgumentParser(
        description='script to display or export images from images.log files')
    parser.add_argument("-i", "--input", help='logfile, containing the images')
    parser.add_argument("-p", "--parallel", help='Flag for enabling multiple processors', default=False)
    parser.add_argument("--width", help='width of image', default=640)
    parser.add_argument("--height", help='height of image', default=480)
    parser.add_argument("--bytes", help='bytes per pixel', default=2)
    args = parser.parse_args()

    input_file = args.input if args.input else images.load_data('image_log')

    if not os.path.exists(input_file):
        sys.exit("[ERROR] path to logfile doesn't exist: {0}".format(input_file))

    camera_bottom = False  # assumes the first image is a top image
    # parse log and save parsed data in a list
    image_data_list = parse_image_log(input_file, args.width, args.height, args.bytes, camera_bottom)

    # export the images from list
    if not args.parallel:
        for image_tuple in image_data_list:
            save_data_to_png(*image_tuple)
    else:
        # export all to in parallel png
        if len(image_data_list) > 0:
            processes = max(mp.cpu_count() - 1, 1)
            print("use {} cores".format(processes))
            pool = mp.Pool(processes=processes)
            pool.starmap(save_data_to_png, image_data_list)
            pool.close()
