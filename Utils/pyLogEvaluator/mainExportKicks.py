#!/usr/bin/python

import BehaviorParser
import json
import math


def init(fileName):
    #fileName = "D:\\AppData\\xampp\\htdocs\\VideoLogLabeling\\log\\2015-07-21-competition-day3-NaoDevils\\half1\\150721-0952-Nao6022\\game.log"
    
    parser = BehaviorParser.BehaviorParser()
    log = BehaviorParser.LogReader(fileName, parser)
    
    # enforce the whole log being parsed (this is necessary for older game logs)
    for frame in log:
      if "BehaviorStateComplete" in frame.messages:
        m = frame["BehaviorStateComplete"]
      if "BehaviorStateSparse" in frame.messages:
        m = frame["BehaviorStateSparse"]
      frame["FrameInfo"]
      
    # we want only the frames which contain BehaviorState
    vlog = filter(lambda f: "BehaviorStateComplete" in f.messages or "BehaviorStateSparse" in f.messages, log)
    
    return vlog

def begin_state(option, state):
  return option["state"] == state and option.stateTime == 0

def begin_option(o, name):
  return name in o and o[name].time == 0
  
def begin_option_state(o, option, state):
  return option in o and begin_state(o[option], state)
  
def option_state(o, option, state):
  return option in o and o[option]["state"].name == state
  
def frameFilter(frame):
  try:
    
    if "BehaviorStateComplete" in frame.messages:
      m, o = frame["BehaviorStateComplete"]
    else:
      m, o = frame["BehaviorStateSparse"]

    # detect the kicks
    action = 'none'
    if "turn_to_attack_direction" in o:
      action = 'turn'
    elif option_state(o, "sidekick", "sidekick_left_foot"):
      action = 'kick_right'
    elif option_state(o, "sidekick", "sidekick_right_foot"):
      action = 'kick_left'
    elif "fast_forward_kick" in o:
      action = 'kick_short'
    elif "kick_with_foot" in o:
      action = 'kick_long'
   
   
    return [frame["FrameInfo"].time/(1000.0*60), action,
      m["robot_pose.x"], m["robot_pose.y"], m["robot_pose.rotation"]*math.pi/180,
      m["ball.position.field.x"], m["ball.position.field.y"]]
    
  except KeyError:
    raise StopIteration
  
def add_entry(intervals, begin_data, end_data):
  pose = {"x":begin_data[2], "y":begin_data[3], "r":begin_data[4]}
  ball = {"x":begin_data[5], "y":begin_data[6]}
  t_begin = begin_data[0]*60
  t_end = end_data[0]*60
  type = begin_data[1]
  
  intervals.append({"type":type, "begin": t_begin, "end": t_end, "pose": pose, "ball": ball})
  
  
  
def run(vlog, fileName):
    #fileName = "./labels.json"
    
    # apply the filter
    data = map(frameFilter, vlog)

    intervals = []
    
    last_entry = data[0]
    
    for d in data:
      # check the state or is last entry
      if last_entry[1] != d[1] or d[0] == data[-1][0]:
        add_entry(intervals, last_entry, d)
        last_entry = d
        
    #fileName = "./labels.json"
    with open(fileName, 'w') as outfile:
      json.dump({"intervals" : intervals}, outfile, indent=4, separators=(',', ': '))
    