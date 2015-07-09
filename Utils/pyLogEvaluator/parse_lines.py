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

from matplotlib.backends import qt_compat
from matplotlib import pyplot as plt

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

if __name__ == "__main__":
  inFile = open("./game.log", "rb")

  log = []
  logm = []

  #while True :
  for i in range(9020):
    try:
      currentFrameNumber = struct.unpack("=l", inFile.read(4))[0]
     
      # name is a '\0' terminated string
      currentName = ""
      c = struct.unpack("=c", inFile.read(1))[0]
      while c != "\0":
        currentName = currentName + c
        c = struct.unpack("=c", inFile.read(1))[0]
      #print currentName
      
      currentSize = struct.unpack("=l", inFile.read(4))[0]
      
      # parse only the TeamMessages
      if currentName == "ScanLineEdgelPerceptTop":
        data = inFile.read(currentSize)
        message = globals()["ScanLineEdgelPercept"]()
        message.ParseFromString(data)
        log.append([currentFrameNumber, message])
        
      elif currentName == "CameraMatrixTop":
        data = inFile.read(currentSize)
        message = globals()["CameraMatrix"]()
        message.ParseFromString(data)
        
        p = m3.Pose3D()
        p.translation = parseVector3(message.pose.translation)
        p.rotation.c1 = parseVector3(message.pose.rotation[0])
        p.rotation.c2 = parseVector3(message.pose.rotation[1])
        p.rotation.c3 = parseVector3(message.pose.rotation[2])
        
        logm.append([currentFrameNumber, p])
        
      else:
        inFile.seek(currentSize,1)
        continue
       
    except Exception as ex:
      print ex
      break

  #cPickle.dump( log, open( "tmp.json", "w" ) )

  cm = logm[-1][1]
  print cm.translation
  for edgel in log[-1][1].edgels:
    w = project(edgel.point.x, edgel.point.y, cm)
    plt.plot(w.x, w.y, "k*")
  plt.show()
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
