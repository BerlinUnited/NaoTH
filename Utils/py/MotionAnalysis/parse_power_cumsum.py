import struct

# protobuf
from naoth.CommonTypes_pb2 import *
from naoth.Framework_Representations_pb2 import *
from naoth.Messages_pb2 import *
from naoth.Representations_pb2 import *

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

    JointID = {
      "HeadPitch": 0,
      "HeadYaw": 1,
      "RShoulderRoll": 2,
      "LShoulderRoll": 3,
      "RShoulderPitch": 4,
      "LShoulderPitch": 5,
      "RElbowRoll": 6,
      "LElbowRoll": 7,
      "RElbowYaw": 8,
      "LElbowYaw": 9,
      "RHipYawPitch": 10,
      "LHipYawPitch": 11,
      "RHipPitch": 12,
      "LHipPitch": 13,
      "RHipRoll": 14,
      "LHipRoll": 15,
      "RKneePitch": 16,
      "LKneePitch": 17,
      "RAnklePitch": 18,
      "LAnklePitch": 19,
      "RAnkleRoll": 20,
      "LAnkleRoll": 21
    }

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
