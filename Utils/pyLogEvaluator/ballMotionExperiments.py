#!/usr/bin/python

import os, sys, getopt

from LogReaderBetter import LogReader
from LogReader import Parser

from matplotlib import pyplot
import numpy

def ballFilter(frame):
  try:
    return [
            frame["FrameInfo"].frameNumber, 
            frame["BallPercept"].bearingBasedOffsetOnField.x,
            frame["BallPercept"].bearingBasedOffsetOnField.y,
            frame["BallPerceptTop"].bearingBasedOffsetOnField.x,
            frame["BallPerceptTop"].bearingBasedOffsetOnField.y
           ]
  except KeyError:
    raise StopIteration


if __name__ == "__main__":

  myParser = Parser()
  myParser.register("BallPerceptTop", "BallPercept")

  log = LogReader("./ball.log", myParser)
  data = map(ballFilter, log)
  
  data_plot = zip(*data)

  #pyplot.plot(data_plot[0], data_plot[3], "ro", label="Ball Position")
  
  #pyplot.plot(data_plot[1], data_plot[2], "ro", label="Ball Position")
  pyplot.plot(data_plot[3], data_plot[4], "bo", label="Ball Position Top")

  pyplot.legend(loc='upper left')
  pyplot.show()
  