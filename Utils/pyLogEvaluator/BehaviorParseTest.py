#!/usr/bin/python

import os, sys, getopt

from LogReader import LogReader
from LogReader import Parser

from matplotlib import pyplot
import numpy

class XABSLSymbols:
  def __init__(self):
    self.values = {}
    self.decimalIdToName = {}
    self.booleanIdToName = {}
    self.enumIdToName = {}

class BehaviorParser(Parser):
  def __init__(self):
    Parser.__init__(self)
    self.symbols = XABSLSymbols()
    self.options = []
    self.current_options = {}
    
  def parseOption(self, o):
    
    if o.type == 0: # Option
      optionComplete = self.options[o.option.id]
      self.current_options[optionComplete] = o.option.timeOfExecution
      
      for so in o.option.activeSubActions:
        self.parseOption(so)
        
    
  def parse(self, name, data):
    self.current_options = {}
    
    if name == 'BehaviorStateComplete':
      message = Parser.parse(self, name, data)
      
      #process options
      for o in message.options:
        self.options.append(o.name)
      
      # process symbols
      for s in message.inputSymbolList.decimal:
        self.symbols.values[s.name] = s.value
        self.symbols.decimalIdToName[s.id] = s.name
      
      for s in message.inputSymbolList.boolean:
        self.symbols.values[s.name] = s.value
        self.symbols.booleanIdToName[s.id] = s.name
      
      for s in message.inputSymbolList.enumerated:
        self.symbols.values[s.name] = s.value
        self.symbols.enumIdToName[s.id] = s.name
      
      return self.symbols.values, self.current_options
      
      
    elif name == 'BehaviorStateSparse':
      message = Parser.parse(self, name, data)
      symbols_values = self.symbols.values.copy()
      
      #process active options
      for o in message.activeRootActions:
        self.parseOption(o)
      
      #process symbols
      for s in message.inputSymbolList.decimal:
        name = self.symbols.decimalIdToName[s.id]
        symbols_values[name] = s.value
      
      for s in message.inputSymbolList.boolean:
        name = self.symbols.booleanIdToName[s.id]
        symbols_values[name] = s.value
        
      for s in message.inputSymbolList.enumerated:
        name = self.symbols.enumIdToName[s.id]
        symbols_values[name] = s.value
      
      return symbols_values, self.current_options
    
    else:
      return Parser.parse(self, name, data)
    

def behavior(frame):
  try:
    m = None
    if "BehaviorStateComplete" in frame.messages:
      #print frame["BehaviorStateComplete"].decimal
      m = frame["BehaviorStateComplete"].values
    else:
      m = frame["BehaviorStateSparse"].values

    return [m["robot_pose.x"], m["robot_pose.y"], m["fall_down_state"]]
    
  except KeyError as k:
    raise StopIteration


if __name__ == "__main__":

  parser = BehaviorParser()
  #fileName = "D:\\RoboCup\\log\\2015-go\\io15-lookaround-audience.log"
  fileName = "./game.log"
  fileName = "D:\\RoboCup\\behavior.log\\rc14-game-htwk-qf\\2-half\\6026\\game.log"
  log = LogReader(fileName, parser)#, filter=headYaw)
  
  print log.names
  
  #b = [ headYaw(f) for f in log]
  vlog = filter(lambda f: "BehaviorStateComplete" in f.messages or "BehaviorStateSparse" in f.messages, log)
  print len(vlog)
  b = map(behavior, vlog)
  #print b
  
  d = zip(*b)
  
  pyplot.ylabel('y')
  pyplot.xlabel('x')
  pyplot.plot(d[2])
  #pyplot.plot(d[0], d[2], label="LKneePitch")
  #pyplot.plot(d[0], d[3], label="RAnklePitch")
  #pyplot.plot(d[0], d[4], label="LAnklePitch")
  #pyplot.legend(loc='upper left')
  pyplot.show()
  