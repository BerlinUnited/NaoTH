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
      self.current_options[optionComplete.name] = { 
        'time': o.option.timeOfExecution,
        'state': optionComplete.states[o.option.activeState],
        'stateTime': o.option.stateTime
      }
      
      for so in o.option.activeSubActions:
        self.parseOption(so)
        
    
  def parse(self, name, data):
    self.current_options = {}
    
    if name == 'BehaviorStateComplete':
      message = Parser.parse(self, name, data)
      
      #process options
      self.options = message.options
      
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
    if "BehaviorStateComplete" in frame.messages:
      m, o = frame["BehaviorStateComplete"]
    else:
      m, o = frame["BehaviorStateSparse"]

    return [m["robot_pose.x"], m["robot_pose.y"], m["fall_down_state"]]
    
  except KeyError as k:
    raise StopIteration


if __name__ == "__main__":

  parser = BehaviorParser()
  fileName = "./game.log"
  log = LogReader(fileName, parser)#, filter=headYaw)
  
  # we want only the frames which contain BehaviorState
  b = [behavior(f) for f in log if "BehaviorStateComplete" in f.messages or "BehaviorStateSparse" in f.messages];
  
  upright = filter(lambda m: m[2] == 1, b)
  fall = filter(lambda m: m[2] != 1, b)
  
  print "step 2"
  du = zip(*upright)
  df = zip(*fall)
  
  pyplot.plot(du[0], du[1], '.')
  pyplot.plot(df[0], df[1], 'o')

  pyplot.ylabel('y')
  pyplot.xlabel('x')
  pyplot.show()
  