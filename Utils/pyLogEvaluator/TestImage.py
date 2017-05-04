#!/usr/bin/python

import os, sys, getopt

from LogReader import LogReader
from LogReader import Parser

from matplotlib import pyplot
import numpy
from PIL import Image


def imageFromProto(message):

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
  
  yuv888 = yuv888.reshape(message.height,message.width,3)
  
  # convert the image to rgb and save it
  img = Image.frombytes('YCbCr', (message.width, message.height), yuv888.tostring())
  return img

  
  
def getImage(frame):
  # we are only interested in top images
  message = frame["Image"]
  return [frame.number, imageFromProto(message)]
  
  
if __name__ == "__main__":

  # initialize the parser
  myParser = Parser()
  # register the protobuf message name for the 'ImageTop'
  myParser.register("ImageTop", "Image")

  fileName = "D:\\RoboCup\\log\\2016-ball\\ball-2016-bottom-left-right.log"
  
  log = LogReader(fileName, myParser)
  
  # get all the images from the logfile
  images = [getImage(f) for f in log] #map(getImageTop, )

  # show the image
  fig = pyplot.figure()
  ax = fig.gca()
  fig.show()
  image = ax.imshow(images[110][1])
  
  pyplot.show()
  quit()

  for i, img in images:
    img = img.convert('RGB')
    
    # show the current image (show the first image or update the data)
    image.set_data(img)
    fig.canvas.draw()
    
    print i
  