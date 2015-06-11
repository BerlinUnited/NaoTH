import struct
import math
import sys
import argparse
import cPickle

import matplotlib
import math2d as m2d
#matplotlib.use('Qt4Agg')
#matplotlib.use('TkAgg')

#from matplotlib.backends import qt_compat
from matplotlib import pyplot as plt

#protobuf
from CommonTypes_pb2 import *
from Framework_Representations_pb2 import *
from Messages_pb2 import *
from Representations_pb2 import *
from google.protobuf import text_format

class Ball:
  def __init__(self):
    self.model = m2d.Vector2()
    self.percept = m2d.Vector2()
    self.was_seen = False
    
class WorldState:
  def __init__(self):
    self.frame = 0
    self.time = 0
    self.ball = Ball()
    self.robot_pose = m2d.Pose2D()
    self.odometry = m2d.Pose2D()

class Kick:
  def __init__(self, frame = 0, name = None):
    self.frame = frame
    self.name = name
    
class BehaviorData:
  def __init__(self):
    self.kicks = {}
    self.state = []
    


def getWorldState(msg, s):
  #s = WorldState()
  s.robot_pose.translation.x = msg["robot_pose.x"]
  s.robot_pose.translation.y = msg["robot_pose.y"]
  s.robot_pose.rotation = msg["robot_pose.rotation"]*math.pi/180 # hack
  s.ball.model.x = msg["ball.x"]
  s.ball.model.y = msg["ball.y"]
  s.ball.percept.x = msg["ball.percept.x"]
  s.ball.percept.y = msg["ball.percept.y"]
  s.ball.was_seen = msg["ball.was_seen"]
  return s
    
    
class BehaviorParser:
  def __init__(self):
    self.behaviorStateComplete = None
    self.frameNumber = -1
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
          self.data.kicks[self.frameNumber] = Kick( self.frameNumber, stateComplete.name )
          
      elif optionName == "situation_attack":
        stateComplete = optionComplete.states[o.option.activeState]
        if stateComplete.name in ["do_kick_with_left_foot","attack_with_left_foot","do_kick_with_right_foot","attack_with_right_foot"] and o.option.stateTime == 0:
          print self.frameNumber, optionName + "."+ stateComplete.name
          self.data.kicks[self.frameNumber] = Kick( self.frameNumber, stateComplete.name )
               
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
  
    currentState = WorldState()
  
    #for i in range(20000):
    while True :
      try:
        currentFrameNumber = struct.unpack("=l", file.read(4))[0]

        # new frame
        if self.frameNumber != currentFrameNumber:
          if self.frameNumber != -1:
            self.data.state.append(currentState)
          self.frameNumber = currentFrameNumber
          currentState = WorldState()
          currentState.frame = self.frameNumber
        
        currentName = ""
        c = struct.unpack("=c", file.read(1))[0]
        while c != "\0":
          currentName = currentName + c
          c = struct.unpack("=c", file.read(1))[0]
          
        currentSize = struct.unpack("=l", file.read(4))[0]
          
        if currentName == "FrameInfo":
          data = file.read(currentSize)
          message = globals()[currentName]()
          message.ParseFromString(data)
          
          # should be the same as self.frameNumber
          #currentState.frame = message.frameNumber
          currentState.time = message.time
          
        elif currentName == "OdometryData":
          data = file.read(currentSize)
          message = globals()[currentName]()
          message.ParseFromString(data)
        
          #print message.pose
          currentState.odometry.translation.x = message.pose.translation.x
          currentState.odometry.translation.y = message.pose.translation.y
          currentState.odometry.rotation = message.pose.rotation
          
        elif currentName == "BehaviorStateComplete":
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

          # safe the data to the current state
          getWorldState(ballPoseState, currentState)
          
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
  print "parse ready"
  cPickle.dump(parser.data, open(args.output, "w"))
