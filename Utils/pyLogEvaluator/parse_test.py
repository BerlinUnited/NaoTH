import struct
import math
from CommonTypes_pb2 import *
from Framework_Representations_pb2 import *
from Messages_pb2 import *
from Representations_pb2 import *
from google.protobuf import text_format
import sys

import matplotlib
matplotlib.use('Qt4Agg')
#matplotlib.use('TkAgg')

from matplotlib.backends import qt_compat
from matplotlib import pyplot as plt

import cPickle

def bla( name ):
  print name
  
def toGlobal(p, b):
  x = b.x
  y = b.y
  b.x = math.cos(p.rotation)*x - math.sin(p.rotation)*y + p.translation.x
  b.y = math.sin(p.rotation)*x + math.cos(p.rotation)*y + p.translation.y
  return b

if __name__ == "__main__":
  inFile = open("./data/game.log", "rb")
  
  log = {}

  while True :
  #for i in range(20000):
    try:
      currentFrameNumber = struct.unpack("l", inFile.read(4))[0]
      
      currentName = ""
      c = struct.unpack("c", inFile.read(1))[0]
      while c != "\0":
        currentName = currentName + c
        c = struct.unpack("c", inFile.read(1))[0]
      #print currentName
      
      #bla(currentName)
      
      currentSize = struct.unpack("l", inFile.read(4))[0]
      
      if currentName != "TeamMessage":
        inFile.seek(currentSize,1)
        continue
       
      data = inFile.read(currentSize)
      
      className = currentName
      if currentName[-3:] == "Top":
        className = currentName[:-3]
      elif currentName == "InertialSensorData":
        className = "DoubleVector"
      elif currentName == "TeamMessage":
        className = "TeamMessageCollection"
      #print className
      
      message = globals()[className]()
      message.ParseFromString(data)
      
      if currentName not in log.keys():
        log[currentName] = []
      log[currentName].append([currentFrameNumber, message])
      
    except Exception as ex:
      print ex
      break

  #cPickle.dump( log, open( "tmp.json", "w" ) )
      
  print log.keys()

  print log["TeamMessage"][1][1].data[2].playerNum
  
  balls = []
  for i in range(5):
  #i = 0
    balls.extend([toGlobal(msg[1].data[i].pose, msg[1].data[i].ballPosition) for msg in log["TeamMessage"] if len(msg[1].data) > 0 and msg[1].data[i].ballAge > 0])
  
  poses = []
  #for i in range(5):
  #  poses.extend([msg[1].data[i].pose for msg in log["TeamMessage"] if len(msg[1].data) > 0])
  
  #x = [p.translation.x for p in poses]
  #y = [p.translation.y for p in poses]
  #r = [p.rotation for p in poses]
  
  x = [b.x for b in balls]
  y = [b.y for b in balls]
  
  #plt.plot(x,y)
  plt.hexbin(x,y, extent=(-4500, 4500, -3000, 3000), cmap=plt.cm.Spectral_r, mincnt=1, gridsize=40, bins='log')
  plt.gca().set_xlim((-4500, 4500))
  plt.gca().set_ylim((-3000, 3000))
  plt.show()