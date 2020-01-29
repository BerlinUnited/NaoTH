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


if __name__ == "__main__":
    get_demo_logfiles()

    parser = ArgumentParser(
        description='script to display or export images from images.log files')
    parser.add_argument("-i", "--input", help='logfile, containing the images', default="logs/images.log")
    parser.add_argument("-p", "--parallel", help='Flag for enabling multiple processors', default=True)
    parser.add_argument("--width", help='width of image', default=640)
    parser.add_argument("--height", help='height of image', default=480)
    parser.add_argument("--bytes", help='bytes per pixel', default=2)
    args = parser.parse_args()

    if not os.path.exists(args.input):
        sys.exit("[ERROR] path to logfile doesn't exist: {0}".format(args.input))

    # create output folder
    logfile_name = Path(args.input).stem
    output_folder = Path(logfile_name)
    output_folder.mkdir(exist_ok=True)

    camera_bottom = False  # assumes the first image is a top image

    # read all data
    image_data_list = list()
    with open(args.input, 'rb') as f:
        while True:
            frame = f.read(4)
            frame_number = int.from_bytes(frame, byteorder='little')

            data = f.read(args.width * args.height * args.bytes)

            # handle the case of incomplete image at the end of the logfile
            if len(data) != args.width * args.height * args.bytes:
                print("read only {0} bytes, but {1} needed.".format(len(data), args.width * args.height * args.bytes))
                break

            img_path = os.path.join(str(output_folder), "{0}.png".format(frame_number))
            if not args.parallel:
                print("save " + img_path)
                save_data_to_png(data, args.width, args.height, img_path, camera_bottom)
            else:
                image_data_list += [(data, args.width, args.height, img_path, camera_bottom)]

            # switch camera id
            camera_bottom = not camera_bottom

    # export all to in parallel png
    if len(image_data_list) > 0:
        processes = max(mp.cpu_count() - 1, 1)
        print("use {} cores".format(processes))
        pool = mp.Pool(processes=processes)
        pool.starmap(save_data_to_png, image_data_list)
        pool.close()
