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
  logp = []

  #while True :
  names = []
  for i in range(9020):
    try:
      currentFrameNumber = struct.unpack("=l", inFile.read(4))[0]
     
      # name is a '\0' terminated string
      currentName = ""
      c = struct.unpack("=c", inFile.read(1))[0]
      while c != "\0":
        currentName = currentName + c
        c = struct.unpack("=c", inFile.read(1))[0]
      if currentName not in names:
        names.append(currentName)
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
      elif currentName == "BehaviorStateComplete":
        data = inFile.read(currentSize)
        message = globals()["BehaviorStateComplete"]()
        message.ParseFromString(data)
      elif currentName == "TeamMessage":
        data = inFile.read(currentSize)
        message = globals()[currentName]()
        message.ParseFromString(data)
        if len(message.data) > 0:
          continue
        logp.append([currentFrameNumber, message])
        
      else:
        inFile.seek(currentSize,1)
        continue
       
    except Exception as ex:
      print ex
      break

  #cPickle.dump( log, open( "tmp.json", "w" ) )

  names.sort()
  print names

  points = []
  commonlength = min(len(logm), len(log))
  for frame in zip(logm[:commonlength], log[:commonlength]):
    if frame[0][0] == frame[1][0]:
      cm = frame[0][1]
      linepoints = [project(edgel.point.x, edgel.point.y, cm) for edgel in frame[1][1].edgels]
      points.append(linepoints)
  cPickle.dump(points, open("lines.pick", "wb"))
#    plt.plot(w.x, w.y, "k*")
#  plt.gca().set_aspect("equal", "datalim")
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
