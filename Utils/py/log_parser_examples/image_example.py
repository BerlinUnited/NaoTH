"""
    expects a logfile that was recorded with the logfile recorder module from robotcontrol
"""
import math
import sys
from argparse import ArgumentParser
from pathlib import Path

import matplotlib.animation as animation
import matplotlib.pyplot as plt
import numpy as np
from PIL import Image
from PIL import PngImagePlugin
from naoth.log import Reader as LogReader
from naoth.log import Parser
from pywget import wget


def get_demo_logfiles():
    base_url = "https://www2.informatik.hu-berlin.de/~naoth/ressources/log/demo_image/"
    logfile_list = ["rc17_ball_far.log"]

    target_dir = Path("logs")
    Path.mkdir(target_dir, exist_ok=True)

    for logfile in logfile_list:
        if not Path(target_dir / logfile).is_file():
            wget.download(base_url + logfile, target_dir)


def get_x_angle(m):
    """
    Returns: return h ? math.acos(c[2].z / h) * (c[2].y > 0 ? -1 : 1) : 0;
    """
    c = m.rotation
    h = math.sqrt(c[2].y * c[2].y + c[2].z * c[2].z)

    if h > 0:
        if c[2].y > 0:
            return math.acos(c[2].z / h) * -1
        else:
            return math.acos(c[2].z / h) * 1
    else:
        return 0
    #


def get_y_angle(m):
    """
    Returns: h ? math.acos(c[0].x / h) * (c[0].z > 0 ? -1 : 1) : 0;
    """
    c = m.rotation
    h = math.sqrt(c[0].x * c[0].x + c[0].z * c[0].z)

    if h > 0:
        if c[0].z > 0:
            return math.acos(c[0].x / h) * -1
        else:
            return math.acos(c[0].x / h) * 1
    else:
        return 0


def image_from_proto(message):
    # read each channel of yuv422 separately
    yuv422 = np.fromstring(message.data, dtype=np.uint8)
    y = yuv422[0::2]
    u = yuv422[1::4]
    v = yuv422[3::4]

    # convert from yuv422 to yuv888
    yuv888 = np.zeros(message.height * message.width * 3, dtype=np.uint8)

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


def save_image_to_png(j, img, cm, target_dir, cam_id, name):
    meta = PngImagePlugin.PngInfo()
    meta.add_text("Message", "rotation maxtrix is saved column wise")
    meta.add_text("logfile", str(name))
    meta.add_text("CameraID", str(cam_id))

    meta.add_text("t_x", str(cm.pose.translation.x))
    meta.add_text("t_y", str(cm.pose.translation.y))
    meta.add_text("t_z", str(cm.pose.translation.z))

    meta.add_text("r_11", str(cm.pose.rotation[0].x))
    meta.add_text("r_21", str(cm.pose.rotation[0].y))
    meta.add_text("r_31", str(cm.pose.rotation[0].z))

    meta.add_text("r_12", str(cm.pose.rotation[1].x))
    meta.add_text("r_22", str(cm.pose.rotation[1].y))
    meta.add_text("r_32", str(cm.pose.rotation[1].z))

    meta.add_text("r_13", str(cm.pose.rotation[2].x))
    meta.add_text("r_23", str(cm.pose.rotation[2].y))
    meta.add_text("r_33", str(cm.pose.rotation[2].z))

    filename = target_dir / (str(j) + ".png")
    img.save(filename, pnginfo=meta)


def export_images(logfile, img):
    """
        creates two folders:
            <logfile name>_top
            <logfile name>_bottom

        and saves the images inside those folders
    """
    logfile_name = Path(logfile).stem
    output_folder_top = Path(logfile_name + "_top")
    output_folder_bottom = Path(logfile_name + "_bottom")

    output_folder_top.mkdir(exist_ok=True)
    output_folder_bottom.mkdir(exist_ok=True)

    if output_folder_top.exists() and output_folder_bottom.exists():
        for i, img_b, img_t, cm_b, cm_t in img:
            img_b = img_b.convert('RGB')
            img_t = img_t.convert('RGB')

            save_image_to_png(i, img_b, cm_b, output_folder_bottom, cam_id=1, name=args.input)
            save_image_to_png(i, img_t, cm_t, output_folder_top, cam_id=0, name=args.input)

            print("saving images from frame ", i)
    else:
        print("target folder does not exist")
        sys.exit(1)


def show_images(img):
    fig, ax = plt.subplots(2)
    fig.suptitle('Images from - ' + args.input)
    ax[0].set_axis_off()
    ax[1].set_axis_off()

    image_container = []
    for i, img_b, img_t, cm_b, cm_t in img:
        img_b = img_b.convert('RGB')
        img_t = img_t.convert('RGB')

        im1 = ax[0].imshow(img_t, animated=True)
        im2 = ax[1].imshow(img_b, animated=True)
        image_container.append([im1, im2])
        print("processing images from frame ", i)

    ani = animation.ArtistAnimation(fig, image_container, interval=50, blit=True,
                                    repeat_delay=1000)
    plt.show()


if __name__ == "__main__":
    get_demo_logfiles()

    parser = ArgumentParser(
        description='script to display or export images from log files')
    parser.add_argument("-i", "--input", help='logfile, containing the images', default="logs/rc17_ball_far.log")
    parser.add_argument("-t", "--task", choices=['show', 'export'], default="export",
                        help='either show or export')

    args = parser.parse_args()

    # initialize the log parser
    myParser = Parser()
    # register the protobuf message name for the 'ImageTop'
    myParser.register("ImageTop", "Image")
    myParser.register("CameraMatrixTop", "CameraMatrix")

    if Path(args.input).is_dir():
        logfile_list = list()
        for file in Path(args.input).glob('*.log'):
            logfile_list.append(file)
    else:
        logfile_list = [args.input]

    if args.task == "export":
        for log in logfile_list:
            # TODO speed up wie in anderen script?
            images = map(get_images, LogReader(log, myParser))
            export_images(log, images)

    if args.task == "show":
        for log in logfile_list:
            images = map(get_images, LogReader(log, myParser))
            show_images(images)
