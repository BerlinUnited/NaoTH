#!/usr/bin/python

import os, sys, getopt

from LogReader import LogReader
from LogReader import Parser

from matplotlib import pyplot
import numpy
from PIL import Image


def parseArguments(argv):
  inputfile = ''
  outputdir = None
  
  try:
    opts, args = getopt.getopt(argv,"hi:o:",["ifile=","outdir="])
  except getopt.GetoptError:
    print 'ExportTopImages.py -i <input file> -o <output directory>'
    sys.exit(2)
    
  for opt, arg in opts:
    if opt == '-h':
      print 'ExportTopImages.py -i <input file> -o <output directory>'
      sys.exit()
    elif opt in ("-i", "--ifile"):
      inputfile = arg
    elif opt in ("-o", "--ofile"):
      outputdir = arg
      
  return inputfile, outputdir

  
def getImageTop(frame):
  # we are only interested in top images
  message = frame["ImageTop"]
  
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
  img = Image.fromstring('YCbCr', (message.width, message.height), yuv888.tostring())
  return [frame.number, img]
  
  
if __name__ == "__main__":

  fileName, targetdir = parseArguments(sys.argv[1:])

  # initialize the parser
  myParser = Parser()
  # register the protobuf message name for the 'ImageTop'
  myParser.register("ImageTop", "Image")

  # show the image
  if targetdir is None:
    fig = pyplot.figure()
    ax = fig.gca()
    fig.show()
    image = None
  
  # get all the images from the logfile
  images = map(getImageTop, LogReader(fileName, myParser))
  
  for i, img in images:
    img = img.convert('RGB')
    
    if targetdir is not None:
      img.save(targetdir+"/"+str(f.number)+".png")
    else:
      # show the current image (show the first image or update the data)
      if image is None:
        image = ax.imshow(img)
      else:
        image.set_data(img)
      fig.canvas.draw()
    
    print i
  