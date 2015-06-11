import struct
import math
import sys

# protobuf
from CommonTypes_pb2 import *
from Framework_Representations_pb2 import *
from Messages_pb2 import *
from Representations_pb2 import *
from google.protobuf import text_format

import matplotlib
matplotlib.use('Qt4Agg')
#matplotlib.use('TkAgg')

from matplotlib.backends import qt_compat
from matplotlib import pyplot as plt

import cPickle


if __name__ == "__main__":
  inFile = open("./game.log", "rb")
  
  log = []

  #while True :
  for i in range(50000):
    try:
      currentFrameNumber = struct.unpack("l", inFile.read(4))[0]
      
      # name is a '\0' terminated string
      currentName = ""
      c = struct.unpack("c", inFile.read(1))[0]
      while c != "\0":
        currentName = currentName + c
        c = struct.unpack("c", inFile.read(1))[0]
      #print currentName
      
      currentSize = struct.unpack("l", inFile.read(4))[0]
      
      # parse only the TeamMessages
      if currentName == "TeamMessage":
        data = inFile.read(currentSize)
        message = globals()[currentName]()
        message.ParseFromString(data)
        log.append([currentFrameNumber, message])
      else:
        inFile.seek(currentSize,1)
        continue
       
    except Exception as ex:
      print ex
      break

  #cPickle.dump( log, open( "tmp.json", "w" ) )
      
  print log[1][1].data[0].playerNum
  
  poses = []
  for i in range(5):
    poses.extend([msg[1].data[i].pose for msg in log if len(msg[1].data) > 0])
  
  for i in range(5):
    #time = [msg[1].data[i].frameInfo.time for msg in log if len(msg[1].data) > 0]
    striker = [msg[1].data[i].user.wasStriker for msg in log if len(msg[1].data) > 0]
    plt.plot(striker)
  plt.gca().set_ylim((-1, 2))
  plt.show()
  
  
  #x = [p.translation.x for p in poses]
  #y = [p.translation.y for p in poses]
  #r = [p.rotation for p in poses]
  
  #plt.plot(x,y)
  #plt.hexbin(x,y, extent=(-4500, 4500, -3000, 3000), cmap=plt.cm.Spectral_r, mincnt=1, gridsize=40, bins='log')
  #plt.gca().set_xlim((-4500, 4500))
  #plt.gca().set_ylim((-3000, 3000))
  plt.show()