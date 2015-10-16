import struct
import sys

# protobuf
from CommonTypes_pb2 import *
from Framework_Representations_pb2 import *
from Messages_pb2 import *
from Representations_pb2 import *
from google.protobuf import text_format

import numpy as np

if __name__ == "__main__":

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
  

  slopes = {}
  filenames = sys.argv[1:]
  joints = JointID.keys()

  joints.sort(key=lambda x:x[1:])

  for filename in filenames:
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
        pass
        break

    
    fm_s = np.array([jd[0]-sjd[0][0] for jd in sjd])
    pad = int(0.1*len(fm_s))
    
    slopes[filename] = {}
    for joint in joints:
      current = [jd[1].electricCurrent[JointID[joint]] for jd in sjd]

      # this is to discard the start and end
      # fit linear model for slopes
      p = np.polyfit(fm_s[pad:-pad], np.cumsum(current)[pad:-pad], 1)
      slopes[filename][joint] = p[0]
 
  
  print ";".join(["filename"] + [joint for joint in joints])
  for filename in filenames:
    print ";".join([filename] + ["{:.6f}".format(slopes[filename][joint]) for joint in joints])
