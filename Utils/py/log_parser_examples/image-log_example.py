#!/usr/bin/python

# this script exports images from the plain image log to png files

import os
import sys
import getopt
import math

import numpy
from PIL import Image
from PIL import PngImagePlugin
import matplotlib.pyplot as plt

# for fast processing
import multiprocessing as mp


def image_from_data(data, width, height):
    # read each channel of yuv422 separately
    yuv422 = numpy.fromstring(data, dtype=numpy.uint8)
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


def save_data_to_png(data, width, height, img_path):
    img = image_from_data(data, width, height)
    img = img.convert('RGB')
    img.save(img_path)
    return True


if __name__ == "__main__":
    # TODO add argparse here
    # TODO figure out how to read the other values
    if len(sys.argv) > 1:
        path = sys.argv[1]
    else:
        sys.exit("[ERROR] need a path to a log file as argument.")

    if not os.path.exists(path):
        sys.exit("[ERROR] path doesn't exist: {0}".format(path))

    # create out directory
    base_file, file_extension = os.path.splitext(path)
    out_dir = base_file

    if os.path.isdir(out_dir):
        sys.exit('[ERROR] directory already exists :\n {0}'.format(out_dir))
    else:
        os.makedirs(out_dir)

    parrallel = True

    width = 640
    height = 480
    bytes = 2

    # read all data
    image_data = []
    with open(path, 'rb') as f:
        j = 0
        while True:
            frame = f.read(4)
            data = f.read(width * height * bytes)

            if len(data) != width * height * bytes:
                print("read only {0} bytes, but {1} needed.".format(len(data), width * height * bytes))
                break

            img_path = os.path.join(out_dir, "{0}.png".format(j))
            if not parrallel:
                print("save " + img_path)
                save_data_to_png(data, width, height, img_path)
            else:
                # TODO fix image data
                image_data += [(data, width, height, img_path)]
            j += 1

    # export all to in parallel png
    if len(image_data) > 0:
        processes = max(mp.cpu_count() - 1, 1)
        print("use {} cores".format(processes))
        pool = mp.Pool(processes=processes)
        pool.starmap(save_data_to_png, image_data)
        pool.close()
