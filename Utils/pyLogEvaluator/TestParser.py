#!/usr/bin/python

import os, sys, getopt

from LogReader import LogReader
from LogReader import Parser

from matplotlib import pyplot
import numpy

JointID = {}
JointID["HeadPitch"] = 0
JointID["HeadYaw"] = 1
JointID["RShoulderRoll"] = 2
JointID["LShoulderRoll"] = 3
JointID["RShoulderPitch"] = 4
JointID["LShoulderPitch"] = 5
JointID["RElbowRoll"] = 6
JointID["LElbowRoll"] = 7
JointID["RElbowYaw"] = 8
JointID["LElbowYaw"] = 9
JointID["RHipYawPitch"] = 10
JointID["LHipYawPitch"] = 11
JointID["RHipPitch"] = 12
JointID["LHipPitch"] = 13
JointID["RHipRoll"] = 14
JointID["LHipRoll"] = 15
JointID["RKneePitch"] = 16
JointID["LKneePitch"] = 17
JointID["RAnklePitch"] = 18
JointID["LAnklePitch"] = 19
JointID["RAnkleRoll"] = 20
JointID["LAnkleRoll"] = 21


def bodyStatus(frame):
  try:
    return [frame["FrameInfo"].time/1000.0, 
            frame["BodyStatus"].currentSum[JointID["RKneePitch"]], 
            frame["BodyStatus"].currentSum[JointID["LKneePitch"]],
            frame["BodyStatus"].currentSum[JointID["RAnklePitch"]], 
            frame["BodyStatus"].currentSum[JointID["LAnklePitch"]]
           ]
  except KeyError:
    raise StopIteration


if __name__ == "__main__":

  fileName = "./game.log"
  log = LogReader(fileName)
  
  #b = [ bodyStatus(f) for f in log]
  b = map(bodyStatus, log)
  
  d = zip(*b)
  
  # plot cumilative control current for each joint
  pyplot.plot(d[0], d[1], label="RKneePitch")
  pyplot.plot(d[0], d[2], label="LKneePitch")
  pyplot.plot(d[0], d[3], label="RAnklePitch")
  pyplot.plot(d[0], d[4], label="LAnklePitch")
  
  pyplot.legend(loc='upper left')
  pyplot.show()
  