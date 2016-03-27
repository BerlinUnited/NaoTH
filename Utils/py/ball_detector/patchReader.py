#!/usr/bin/python

import os, sys, getopt, math

from LogReader import LogReader
from LogReader import Parser

from matplotlib import pyplot
import numpy
from PIL import Image
from PIL import PngImagePlugin

def parseArguments(argv):
  inputfile = ''
  outputdir = None
  
  try:
    opts, args = getopt.getopt(argv,"hi:",["ifile="])
  except getopt.GetoptError:
    print 'ExportTopImages.py -i <input file>'
    sys.exit(2)
    
  for opt, arg in opts:
    if opt == '-h':
      print 'ExportTopImages.py -i <input file>'
      sys.exit()
    elif opt in ("-i", "--ifile"):
      inputfile = arg
      
  return inputfile

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
  img = Image.fromstring('YCbCr', (message.width, message.height), yuv888.tostring())
  return img
  
  
def getImages(frame):
  # we are only interested in top images
  imageTop = frame["ImageTop"]
  imageBottom = frame["Image"]
  cmBottom = frame["CameraMatrix"]
  cmTop = frame["CameraMatrixTop"]
  return [frame.number, imageFromProto(imageBottom), imageFromProto(imageTop), cmBottom, cmTop]
  
def getPatches(frame):
  ballCandidates = frame["BallCandidates"]
  ballCandidatesTop = frame["BallCandidatesTop"]
  print len(ballCandidates.patches), len(ballCandidatesTop.patches)
  return [ballCandidatesTop]

  
def readAllPatchesFromLog(fileName):
  # initialize the parser
  myParser = Parser()
  # register the protobuf message name for the 'ImageTop'
  myParser.register("ImageTop", "Image")
  myParser.register("BallCandidatesTop", "BallCandidates")
  myParser.register("CameraMatrixTop", "CameraMatrix")

  # get all the images from the logfile
  #images = map(getPatches, LogReader(fileName, myParser))
  
  patches = []
  for frame in LogReader(fileName, myParser):
    ballCandidates = frame["BallCandidates"]
    for p in ballCandidates.patches:
      data = numpy.fromstring(p.data, dtype=numpy.uint8)
      patches.append(data);
      
    ballCandidatesTop = frame["BallCandidatesTop"]
    for p in ballCandidatesTop.patches:
      data = numpy.fromstring(p.data, dtype=numpy.uint8)
      patches.append(data);
  
  return patches
  
if __name__ == "__main__":

  fileName = parseArguments(sys.argv[1:])
  print fileName
  
  patches = readAllPatchesFromLog(fileName)
  print len(patches)
  
  