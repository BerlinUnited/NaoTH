import struct
import math
from CommonTypes_pb2 import *
from Framework_Representations_pb2 import *
from Messages_pb2 import *
from Representations_pb2 import *
from google.protobuf import text_format
import sys
import argparse

import matplotlib
#matplotlib.use('Qt4Agg')
#matplotlib.use('TkAgg')

#from matplotlib.backends import qt_compat
from matplotlib import pyplot as plt

import cPickle

class BehaviorData:
  def __init__(self):
    self.kicks = []
    self.ballpose = []

class BehaviorParser:
  def __init__(self):
    self.behaviorStateComplete = None
    self.frameNumber = 0
    self.data = BehaviorData()
    
  def printOptions(self, o):
    #print(o.type)
    
    if o.type == 0: # Option
      optionComplete = self.behaviorStateComplete.options[o.option.id]
      optionName = optionComplete.name
      
      #stateTime
      if optionName == "situation_sidekick":
        stateComplete = optionComplete.states[o.option.activeState]
        if stateComplete.name in ["sidekick_to_right","sidekick_to_left"] and o.option.stateTime == 0:
          print self.frameNumber, optionName + "."+ stateComplete.name
          self.data.kicks.append( [ self.frameNumber, stateComplete.name ] )
          
      elif optionName == "situation_attack":
        stateComplete = optionComplete.states[o.option.activeState]
        if stateComplete.name in ["do_kick_with_left_foot","attack_with_left_foot","do_kick_with_right_foot","attack_with_right_foot"] and o.option.stateTime == 0:
          print self.frameNumber, optionName + "."+ stateComplete.name
          self.data.kicks.append( [ self.frameNumber, stateComplete.name ] )
          
#, "sidekick", "fast_forward_kick"]        
      for so in o.option.activeSubActions:
        self.printOptions(so)

  def parse(self, file):

    ids_decimal = {}
    ids_decimal["robot_pose.x"] = -1
    ids_decimal["robot_pose.y"] = -1
    ids_decimal["robot_pose.rotation"] = -1
    ids_decimal["ball.x"] = -1
    ids_decimal["ball.y"] = -1
    ids_decimal["ball.percept.x"] = -1
    ids_decimal["ball.percept.y"] = -1
    
    ids_boolean = {}
    ids_boolean["ball.was_seen"] = -1
  
    #for i in range(20000):
    while True :
      try:
        currentFrameNumber = struct.unpack("=l", file.read(4))[0]

        self.frameNumber = currentFrameNumber
        
        currentName = ""
        c = struct.unpack("=c", file.read(1))[0]
        while c != "\0":
          currentName = currentName + c
          c = struct.unpack("=c", file.read(1))[0]
          
        currentSize = struct.unpack("=l", file.read(4))[0]
          
        if currentName == "BehaviorStateComplete":
          data = file.read(currentSize)
          message = globals()[currentName]()
          message.ParseFromString(data)
          #print(message)
          
          #print(message.options[7])
          self.behaviorStateComplete = message;
          
          for ds in message.inputSymbolList.decimal:
            if ds.name in ids_decimal.keys():
              ids_decimal[ds.name] = ds.id
          
          for ds in message.inputSymbolList.boolean:
            if ds.name in ids_boolean.keys():
              ids_boolean[ds.name] = ds.id
              
          print ids_decimal
          print ids_boolean
          
        elif currentName == "BehaviorStateSparse":
          data = file.read(currentSize)
          message = globals()[currentName]()
          message.ParseFromString(data)

          ballPoseState = {}

          #default values
          for sn in ids_decimal.keys():
            ballPoseState[sn] = self.behaviorStateComplete.inputSymbolList.decimal[ids_decimal[sn]].value
            
          for sn in ids_decimal.keys():
            for s in message.inputSymbolList.decimal:
              if s.id == ids_decimal[sn]:
                ballPoseState[sn] = s.value
          
          for sn in ids_boolean.keys():
            ballPoseState[sn] = self.behaviorStateComplete.inputSymbolList.boolean[ids_boolean[sn]].value
          
          for sn in ids_boolean.keys():
            for s in message.inputSymbolList.boolean:
              if s.id == ids_boolean[sn]:
                ballPoseState[sn] = s.value
          
          #print ballPoseState
          
          self.data.ballpose.append([self.frameNumber, ballPoseState])
          
          for o in message.activeRootActions:
            self.printOptions(o)
        else:
          file.seek(currentSize,1)
          continue

      except Exception as ex:
        print ex
        break
        
if __name__ == "__main__":
  argparser = argparse.ArgumentParser(description="parse log file for balls and kicks and output to pickle file")
  argparser.add_argument("filename", help="log file", nargs=1)
  argparser.add_argument("-o", "--output", help="name for the output file", type=str, default="ballkick.cPickle")
  
  args = argparser.parse_args()
 
  inFile = open(args.filename[0], "rb")
  parser = BehaviorParser()
  parser.parse(inFile)
  
  cPickle.dump(parser.data, open(args.output, "w"))
