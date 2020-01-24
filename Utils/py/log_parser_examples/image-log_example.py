"""
    this script exports images from the plain image log to png files
"""
import multiprocessing as mp
import os
import sys
from argparse import ArgumentParser
from pathlib import Path

import numpy
from PIL import Image
from pywget import wget


def get_demo_logfiles():
    base_url = "https://www2.informatik.hu-berlin.de/~naoth/ressources/log/demo_image/"

    logfile_list = ["images.log"]
    # taken from /vol/repl261-vol4/naoth/logs/2019-11-21_Koeln/
    # 2019-11-21_16-20-00_Berlin United_vs_Nao_Devils_Dortmund_half1/game_logs/1_96_Nao0377_191122-0148

    target_dir = Path("logs")
    Path.mkdir(target_dir, exist_ok=True)

    for logfile in logfile_list:
        if not Path(target_dir / logfile).is_file():
            wget.download(base_url + logfile, target_dir)


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
    # TODO figure out how to read the other values
    # TODO save metadata to images
    # TODO differentiate between top and bottom
    get_demo_logfiles()

    parser = ArgumentParser(
        description='script to display or export images from images.log files')
    parser.add_argument("-i", "--input", help='logfile, containing the images', default="logs/images.log")
    parser.add_argument("-p", "--parallel", help='Flag for enabling multiple processors', default=True)

    args = parser.parse_args()

    if not os.path.exists(args.input):
        sys.exit("[ERROR] path to logfile doesn't exist: {0}".format(args.input))

    # create output folder
    logfile_name = Path(args.input).stem
    output_folder = Path(logfile_name)
    output_folder.mkdir(exist_ok=True)

    width = 640
    height = 480
    bytes = 2

    # read all data
    image_data = []
    with open(args.input, 'rb') as f:
        while True:
            frame = f.read(4)
            frame_number = int.from_bytes(frame, byteorder='little')

            data = f.read(width * height * bytes)

            # TODO guess the cameraID here

            # handle the case of incomplete image at the end of the logfile
            if len(data) != width * height * bytes:
                print("read only {0} bytes, but {1} needed.".format(len(data), width * height * bytes))
                break

            img_path = os.path.join(str(output_folder), "{0}.png".format(frame_number))
            if not args.parallel:
                print("save " + img_path)
                save_data_to_png(data, width, height, img_path)
            else:
                image_data += [(data, width, height, img_path)]

    # export all to in parallel png
    if len(image_data) > 0:
        processes = max(mp.cpu_count() - 1, 1)
        print("use {} cores".format(processes))
        pool = mp.Pool(processes=processes)
        pool.starmap(save_data_to_png, image_data)
        pool.close()
