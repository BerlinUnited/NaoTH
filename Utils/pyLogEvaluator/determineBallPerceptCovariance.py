import struct
import math
import sys
import numpy as np

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

def getOpticalCenterX(camInfo):
    return camInfo.resolutionWidth/2

def getOpticalCenterY(camInfo):
    return camInfo.resolutionHeight/2

def getFocalLength(camInfo):
  resolutionWidth = camInfo.resolutionWidth
  resolutionHeight = camInfo.resolutionHeight
  openingAngleDiagonal = camInfo.openingAngleDiagonal/180*math.pi
 
  d2 = resolutionWidth * resolutionWidth + resolutionHeight * resolutionHeight
  halfDiagLength = 0.5 * math.sqrt(d2)
  return halfDiagLength / math.tan(0.5 * openingAngleDiagonal)

def pixelToAngles(camInfo,imgX,imgY):
   x = getFocalLength(camInfo)
   y = -imgX + getOpticalCenterX(camInfo)
   z = -imgY + getOpticalCenterY(camInfo)
   return (math.atan2(y,x),math.atan2(z,x))

if __name__ == "__main__":
  inFile = open("./game.log", "rb")

  log_BallPercept  = []
  log_CameraInfo   = []
  log_BallPerceptTop  = []
  log_CameraInfoTop   = []

  player_number = -1

  while True :
  #for i in range(9020):
    try:
      currentFrameNumber = struct.unpack("=l", inFile.read(4))[0]
     
      # name is a '\0' terminated string
      currentName = ""
      # read a char until '\0' char appears
      c = struct.unpack("=c", inFile.read(1))[0]
      while c != "\0":
        currentName = currentName + c
        c = struct.unpack("=c", inFile.read(1))[0]

      currentSize = struct.unpack("=l", inFile.read(4))[0]
      
      if currentName == "BallPercept":
        data = inFile.read(currentSize)
        message = globals()["BallPercept"]()
        message.ParseFromString(data)
        log_BallPercept.append([currentFrameNumber, message])

      elif currentName == "BallPerceptTop":
        data = inFile.read(currentSize)
        message = globals()["BallPercept"]()
        message.ParseFromString(data)
        log_BallPerceptTop.append([currentFrameNumber, message])

      elif currentName == "CameraInfo":
        data = inFile.read(currentSize)
        message = globals()["CameraInfo"]()
        message.ParseFromString(data)
        log_CameraInfo.append([currentFrameNumber, message])

      elif currentName == "CameraInfoTop":
        data = inFile.read(currentSize)
        message = globals()["CameraInfo"]()
        message.ParseFromString(data)
        log_CameraInfoTop.append([currentFrameNumber, message])
        
      else:
        inFile.seek(currentSize,1)
        continue
       
    except Exception as ex:
      print("Exception occured")
      print(ex)
      break

  angles    = []
  anglesTop = []

  for ball, ballTop, camInfo, camInfoTop in zip(log_BallPercept, log_BallPerceptTop, log_CameraInfo, log_CameraInfoTop):
      if ball[1].ballWasSeen:
          angles.append(pixelToAngles(camInfo[1],ball[1].centerInImage.x,ball[1].centerInImage.y))

      if ballTop[1].ballWasSeen:
          anglesTop.append(pixelToAngles(camInfoTop[1],ballTop[1].centerInImage.x,ballTop[1].centerInImage.y))
  
  if len(angles) > 0:
      data = np.array(angles).transpose()
      cov  = np.cov(data)
      print("bottom cam number of samples:")
      print(len(angles))
      print("covariance matrix of the bottom cam:")
      print(cov)

  if len(anglesTop) > 0:
      dataTop = np.array(anglesTop).transpose()
      covTop  = np.cov(dataTop)
      print("top cam number of samples:")
      print(len(anglesTop))
      print("covariance matrix of the top cam:")
      print(covTop)

#  plt.show()
#  poses = []
#  for i in range(5):
#    poses.extend([msg[1].data[i].pose for msg in log if len(msg[1].data) > 0])
#  
#  for i in range(5):
#    #time = [msg[1].data[i].frameInfo.time for msg in log if len(msg[1].data) > 0]
#    striker = [msg[1].data[i].user.wasStriker for msg in log if len(msg[1].data) > 0]
#    plt.plot(striker)
#  plt.gca().set_ylim((-1, 2))
#  plt.show()
  
  
  #x = [p.translation.x for p in poses]
  #y = [p.translation.y for p in poses]
  #r = [p.rotation for p in poses]
  
  #plt.plot(x,y)
  #plt.hexbin(x,y, extent=(-4500, 4500, -3000, 3000), cmap=plt.cm.Spectral_r, mincnt=1, gridsize=40, bins='log')
  #plt.gca().set_xlim((-4500, 4500))
  #plt.gca().set_ylim((-3000, 3000))
  plt.show()
