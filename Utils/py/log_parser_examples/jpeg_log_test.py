"""
    expects a logfile that was recorded with the logfile recorder module from robotcontrol
"""
import sys
import io

import numpy as np

from PIL import Image
#from PIL import PngImagePlugin

from naoth.log import Parser
from naoth.log import Reader as LogReader


def image_from_proto(message):
    
    # hack: 
    if message.format == message.JPEG:
        # unpack JPG
        img = Image.open(io.BytesIO(message.data))
    
        # HACK: for some reason the decoded image is inverted ...
        yuv422 = 255 - np.array(img, dtype=np.uint8)
        
        # flatten the image to get the same data formal like a usual yuv422
        yuv422 = yuv422.reshape(message.height * message.width * 2)
    else:
        # read each channel of yuv422 separately
        yuv422 = np.frombuffer(message.data, dtype=np.uint8)
    
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
    
    # convert the image to rgb
    img = Image.frombytes('YCbCr', (message.width, message.height), yuv888.tobytes())
    
    return img


if __name__ == "__main__":

    # register jpg image messages
    myParser = Parser()
    myParser.register("ImageJPEG"   , "Image")
    myParser.register("ImageJPEGTop", "Image")

    with LogReader("game.log", myParser) as reader:
        for frame in reader:

            if "ImageJPEG" in frame.get_names():
                message = frame["ImageJPEG"]
                img = image_from_proto(message)
                img.show()
            
            if "ImageJPEGTop" in frame.get_names():
                message = frame["ImageJPEGTop"]
                img = image_from_proto(message)
                img.show()
                
                