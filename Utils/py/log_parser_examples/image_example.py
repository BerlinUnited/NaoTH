#!/usr/bin/python
from  argparse import ArgumentParser
import sys
from pathlib import Path
import math
import matplotlib.animation as animation
import matplotlib.pyplot as plt
import numpy as np
from PIL import Image
from PIL import PngImagePlugin

from naoth.LogReader import LogReader
from naoth.LogReader import Parser


def get_x_angle(m):
    c = m.rotation
    h = math.sqrt(c[2].y * c[2].y + c[2].z * c[2].z)
    
    if h > 0:
        if c[2].y > 0:
            return math.acos(c[2].z / h) * -1
        else:
            return math.acos(c[2].z / h) * 1
    else:
        return 0
    # return h ? math.acos(c[2].z / h) * (c[2].y > 0 ? -1 : 1) : 0;


def get_y_angle(m):
    c = m.rotation
    h = math.sqrt(c[0].x * c[0].x + c[0].z * c[0].z)
    
    if h > 0:
        if c[0].z > 0:
            return math.acos(c[0].x / h) * -1
        else:
            return math.acos(c[0].x / h) * 1
    else:
        return 0
    
    # return h ? math.acos(c[0].x / h) * (c[0].z > 0 ? -1 : 1) : 0;


def image_from_proto(message):
    # read each channel of yuv422 separately
    yuv422 = np.fromstring(message.data, dtype=np.uint8)
    y = yuv422[0::2]
    u = yuv422[1::4]
    v = yuv422[3::4]

    # convert from yuv422 to yuv888
    yuv888 = np.zeros(message.height*message.width*3, dtype=np.uint8)

    yuv888[0::3] = y
    yuv888[1::6] = u
    yuv888[2::6] = v
    yuv888[4::6] = u
    yuv888[5::6] = v
    
    yuv888 = yuv888.reshape((message.height, message.width, 3))

    # convert the image to rgb and save it
    img = Image.frombytes('YCbCr', (message.width, message.height), yuv888.tostring())
    return img
    
    
def get_images(frame):
    # TODO handle the case that no image is recorded
    # we are only interested in top images
    image_top = frame["ImageTop"]
    image_bottom = frame["Image"]
    cm_bottom = frame["CameraMatrix"]
    cm_top = frame["CameraMatrixTop"]
    return [frame.number, image_from_proto(image_bottom),
            image_from_proto(image_top), cm_bottom, cm_top]


def save_image_to_png(j, img, cm, target_dir):
    pitch = get_y_angle(cm.pose)
    roll = get_x_angle(cm.pose)
    meta = PngImagePlugin.PngInfo()
    meta.add_text("pitch", str(pitch))
    meta.add_text("roll", str(roll))

    filename = target_dir / (str(j) + ".png")
    img.save(filename, pnginfo=meta)


if __name__ == "__main__":
    parser = ArgumentParser(
        description='script to display or export images from log files')
    parser.add_argument("-i", "--input", help='logfile, containing the images', default="logs/rc17_ball_far.log")
    parser.add_argument("-t", "--task", choices=['show', 'export'], default="show",
                        help='either show or export')
    parser.add_argument("-o", "--output", type=str, default=".", help='output folder')

    args = parser.parse_args()

    # initialize the log parser
    myParser = Parser()
    # register the protobuf message name for the 'ImageTop'
    myParser.register("ImageTop", "Image")
    myParser.register("CameraMatrixTop", "CameraMatrix")
    
    # get all the images from the logfile
    images = map(get_images, LogReader(args.input, myParser))

    if args.task == "export":
        output_folder = Path(args.output)
        if output_folder.exists():
            for i, imgB, imgT, cmB, cmT in images:
                imgB = imgB.convert('RGB')
                imgT = imgT.convert('RGB')

                output_folder_top = output_folder / "top"
                output_folder_bottom = output_folder / "bottom"

                output_folder_top.mkdir(exist_ok=True)
                output_folder_bottom.mkdir(exist_ok=True)

                save_image_to_png(i, imgB, cmB, output_folder_bottom)
                save_image_to_png(i, imgT, cmT, output_folder_top)

                print("saving images from frame ", i)
        else:
            print("target folder does not exist")
            sys.exit(1)

    if args.task == "show":
        fig, ax = plt.subplots(2)
        fig.suptitle('NaoTH Image Log')
        ax[0].set_axis_off()
        ax[1].set_axis_off()

        image_container = []
        for i, imgB, imgT, cmB, cmT in images:
            imgB = imgB.convert('RGB')
            imgT = imgT.convert('RGB')

            im1 = ax[0].imshow(imgT, animated=True)
            im2 = ax[1].imshow(imgB, animated=True)
            image_container.append([im1, im2])
            print("processing images from frame ", i)

        ani = animation.ArtistAnimation(fig, image_container, interval=50, blit=True,
                                        repeat_delay=1000)
        plt.show()
