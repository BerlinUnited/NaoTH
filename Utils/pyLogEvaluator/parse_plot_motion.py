import struct
import math
import sys
import math3d as m3
import math2d as m2

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
import sys
import numpy as np
import cPickle

def getFocalLength():
  resolutionWidth = 640
  resolutionHeight = 480
  openingAngleDiagonal = 72.6/180*math.pi
  
  d2 = resolutionWidth * resolutionWidth + resolutionHeight * resolutionHeight
  halfDiagLength = 0.5 * math.sqrt(d2)
  return halfDiagLength / math.tan(0.5 * openingAngleDiagonal)

def project(x,y,cm):
  v = m3.Vector3()
  v.x = getFocalLength()
  v.y = 320 - x
  v.z = 240 - y
  
  v = cm.rotation*v
  result = m2.Vector2()
  result.x = v.x
  result.y = v.y
  result = result*(cm.translation.z/(-v.z))
  result.x = result.x + cm.translation.x
  result.y = result.y + cm.translation.y
  return result
  
def parseVector3(msg):
  return m3.Vector3(msg.x,msg.y,msg.z)

def smooth(x,window_len=11,window='hanning'):
  if x.ndim != 1:
    raise ValueError, "smooth only accepts 1 dimension arrays."
  if x.size < window_len:
    return x
#    raise ValueError, "Input vector needs to be bigger than window size."
  if window_len<3:
    return x
  if not window in ['flat', 'hanning', 'hamming', 'bartlett', 'blackman']:
    raise ValueError, "Window is on of 'flat', 'hanning', 'hamming', 'bartlett', 'blackman'"
  s=np.r_[2*x[0]-x[window_len-1::-1],x,2*x[-1]-x[-1:-window_len:-1]]
  if window == 'flat': #moving average
    w=np.ones(window_len,'d')
  else:  
    w=eval('np.'+window+'(window_len)')
  y=np.convolve(w/w.sum(),s,mode='same')
  return y[window_len:-window_len+1]

if __name__ == "__main__":
  inFile = open(sys.argv[1], "rb")
    
  rcParams["font.family"] = "serif"
  rcParams["xtick.labelsize"] = 6
  rcParams["ytick.labelsize"] = 6
  rcParams["axes.labelsize"] = 6
  rcParams["axes.titlesize"] = 6
  rcParams["legend.fontsize"] = 6

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


  lastID = 0
  scd_start = []
  scd_end = []
  for mr in mrd:
    try:
      if mr[1].walkRequest.stepControl.stepID != lastID:
        if mr[1].walkRequest.stepControl.stepID != 0:
          scd_start.append(mr)
        else:
          scd_end.append(mr)
        lastID = mr[1].walkRequest.stepControl.stepID
    except:
      pass

  print len(scd_start), len(scd_end)

  plotJoints = sorted([key for key in JointID.keys() if "Hip" in key or "Knee" in key or "Ankle" in key])
  plotJoints = list(set([key[1:] for key in plotJoints]))

  for i in range(len(plotJoints)):
    for side in ["L", "R"]:
      joint = side + plotJoints[i]
      if side == "L":
        plt.subplot(len(plotJoints),2,2*i+1)
      elif side == "R":
        plt.subplot(len(plotJoints),2,2*(i+1))
      plt.grid()
      sidename = "left" if side == "L" else "right"
      plt.title(sidename+": "+joint)
      # motorjointdata
      fm_m = [jd[0] for jd in mjd]
      jointpos_m = [jd[1].position[JointID[joint]] for jd in mjd]
      jointdp_m = [jd[1].dp[JointID[joint]] for jd in mjd]
      jointddp_m = [jd[1].ddp[JointID[joint]] for jd in mjd]
      #sensorjointdata
      fm_s = [jd[0] for jd in sjd]
      jointpos_s = [jd[1].jointData.position[JointID[joint]] for jd in sjd]
      jointdp_s = [jd[1].jointData.dp[JointID[joint]] for jd in sjd]
      jointddp_s = [jd[1].jointData.ddp[JointID[joint]] for jd in sjd]
      jointcurrent_s = [jd[1].electricCurrent[JointID[joint]] for jd in sjd]
      plt.plot(fm_m, jointpos_m, "b-", label=joint+"_pos_motor")
      plt.plot(fm_s, jointpos_s, "c-", label=joint+"_pos_sensor")
      plt.plot(fm_m, jointdp_m, "r-", label=joint+"_dp_motor")
      plt.plot(fm_m, jointdp_s, "y-", label=joint+"_dp_sensor")
      plt.plot(fm_s, jointcurrent_s, "k-", label=joint+"_curr_sensor")
      plt.legend()
      for sc in scd_start:
        plt.plot([sc[0], sc[0]], [-20,20], "m-")
      for sc in scd_end:
        plt.plot([sc[0], sc[0]], [-20,20], "g-")
  plt.show()
