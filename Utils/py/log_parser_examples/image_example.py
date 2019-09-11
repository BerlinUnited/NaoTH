#!/usr/bin/python
import os
import sys
import getopt
import math

from naoth.LogReader import LogReader
from naoth.LogReader import Parser

import numpy
from PIL import Image
from PIL import PngImagePlugin


def parse_arguments(argv):
    input_file = ''
    output_dir = None
    
    try:
        opts, args = getopt.getopt(argv, "hi:o:", ["ifile=", "outdir="])
    except getopt.GetoptError:
        print('ExportImages.py -i <input file> -o <output directory>')
        sys.exit(2)
    if not opts:
        print('ExportImages.py -i <input file>')
        sys.exit(2)    
    for opt, arg in opts:
        if opt == '-h':
            print('ExportImages.py -i <input file> -o <output directory>')
            sys.exit()
        elif opt in ("-i", "--ifile"):
            input_file = arg
        elif opt in ("-o", "--ofile"):
            output_dir = arg
            
    return input_file, output_dir


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
    yuv422 = numpy.fromstring(message.data, dtype=numpy.uint8)
    y = yuv422[0::2]
    u = yuv422[1::4]
    v = yuv422[3::4]

    # convert from yuv422 to yuv888
    yuv888 = numpy.zeros(message.height*message.width*3, dtype=numpy.uint8)
    yuv888[0::3] = y
    yuv888[1::6] = u
    yuv888[2::6] = v
    yuv888[4::6] = u
    yuv888[5::6] = v
    
    yuv888 = yuv888.reshape(message.height, message.width, 3)
    
    # convert the image to rgb and save it
    img = Image.frombytes('YCbCr', (message.width, message.height), yuv888.tostring())
    return img
    
    
def get_images(frame):
    # we are only interested in top images
    image_top = frame["ImageTop"]
    image_bottom = frame["Image"]
    cm_bottom = frame["CameraMatrix"]
    cm_top = frame["CameraMatrixTop"]
    return [frame.number, image_from_proto(image_bottom), image_from_proto(image_top), cm_bottom, cm_top]


def save_image_to_png(j, img, cm, target_dir):
    pitch = get_y_angle(cm.pose)
    roll = get_x_angle(cm.pose)
    meta = PngImagePlugin.PngInfo()
    meta.add_text("pitch", str(pitch))
    meta.add_text("roll", str(roll))
    img.save(target_dir+"/"+str(j)+".png", pnginfo=meta)
    

if __name__ == "__main__":

    fileName, target_dir = parse_arguments(sys.argv[1:])
    
    # initialize the parser
    myParser = Parser()
    # register the protobuf message name for the 'ImageTop'
    myParser.register("ImageTop", "Image")
    myParser.register("CameraMatrixTop", "CameraMatrix")
    
    # get all the images from the logfile
    images = map(get_images, LogReader(fileName, myParser))
    
    for i, imgB, imgT, cmB, cmT in images:
        imgB = imgB.convert('RGB')
        imgT = imgT.convert('RGB')

        if target_dir is not None:
        
            targetTop = target_dir + "/top"
            targetBottom = target_dir + "/bottom"
        
            if not os.path.exists(targetTop):
                os.makedirs(targetTop)
            if not os.path.exists(targetBottom):
                os.makedirs(targetBottom)
        
            save_image_to_png(i, imgB, cmB, targetBottom)
            save_image_to_png(i, imgT, cmT, targetTop)
        else:
            print("Target dir not set")
        print(i)
