import struct
import sys

# protobuf
from CommonTypes_pb2 import *
from Framework_Representations_pb2 import *
from Messages_pb2 import *
from Representations_pb2 import *
from google.protobuf import text_format

import matplotlib
#matplotlib.use('Qt4Agg')
#matplotlib.use('TkAgg')
#from matplotlib.backends import qt_compat
from matplotlib import pyplot as plt
from matplotlib import rcParams
import numpy as np

if __name__ == "__main__":
    
  rcParams["font.family"] = "serif"
  rcParams["xtick.labelsize"] = 10
  rcParams["ytick.labelsize"] = 10
  rcParams["axes.labelsize"] = 10
  rcParams["axes.titlesize"] = 10
  rcParams["legend.fontsize"] = 10

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
  

  slopes = []

  for filename in sys.argv[1:]:
    inFile = open(filename, "rb")
    
    mjd = []
    sjd = []
    mrd = []

    while True :
      try:
        currentFrameNumber = struct.unpack("=l", inFile.read(4))[0]
       
        # name is a '\0' terminated string
        currentName = ""
        c = struct.unpack("=c", inFile.read(1))[0]
        while c != "\0":
          currentName = currentName + c
          c = struct.unpack("=c", inFile.read(1))[0]

        currentSize = struct.unpack("=l", inFile.read(4))[0]
      
        if currentName == "SensorJointData":
          data = inFile.read(currentSize)
          msg = globals()["SensorJointData"]()
          msg.ParseFromString(data)
          sjd.append([currentFrameNumber, msg])
        elif currentName == "MotorJointData":
          data = inFile.read(currentSize)
          msg = globals()["JointData"]()
          msg.ParseFromString(data)
          mjd.append([currentFrameNumber, msg])
        elif currentName == "MotionRequest":
          data = inFile.read(currentSize)
          msg = globals()["MotionRequest"]()
          msg.ParseFromString(data)
          mrd.append([currentFrameNumber, msg])
          
        else:
          inFile.seek(currentSize,1)
          continue
         
      except Exception as ex:
        print ex
        break

    
    fm_s = np.array([jd[0]-sjd[0][0] for jd in sjd])
  
    # filter
    filters = ["Knee", "Ankle", "Hip"]

    currents = [[jd[1].electricCurrent[JointID[joint]] for jd in sjd] for joint in JointID.keys() if sum([1 for fil in filters if fil in joint]) > 0]
    current = np.sum(np.vstack(currents), 0)

    # plot data
    plt.plot(fm_s, np.cumsum(current), label=filename)
   
    # this is to discard the start and end
    pad = int(0.1*len(fm_s))
    # fit linear model for slopes
    p = np.polyfit(fm_s[pad:-pad], np.cumsum(current)[pad:-pad], 1)
    slopes.append([filename, p[0]])
    # plot fit
    x = np.linspace(fm_s[pad], fm_s[-pad], 100)
    y = np.polyval(p, x)
    plt.plot(x,y, "k:")
  
  slopes.sort(key=lambda x:x[1])
  # norm to lowest
  norm = slopes[0][1]
  for i in range(len(slopes)):
    slopes[i][1] = slopes[i][1] / norm
  for filename, m in slopes:
    print filename, m

  plt.grid()
  plt.legend()
  plt.show()
