#!/usr/bin/python

import BehaviorParser
import json



def init(fileName):
    #fileName = "D:\\Projects\\2016-web-video-player\\log\\20150426-Game-NaoDevils\\half1\\150426-1208-Nao6022\\game.log"
    
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
    if "situation_based_striker" in o:
      if o["situation_based_striker"]["state"] == "turn":
        action = 'turn'
      elif option_state(o, "sidekick", "sidekick_left_foot"):
        action = 'kick_right'
      elif option_state(o, "sidekick", "sidekick_right_foot"):
        action = 'kick_left'
      elif "fast_forward_kick" in o:
        action = 'kick_short'
      elif "kick_with_foot" in o:
        action = 'kick_long'
        
    return [frame["FrameInfo"].time/(1000.0*60), action]
    
  except KeyError:
    raise StopIteration
    
  
def run(vlog, fileName):
    
    # apply the filter
    data = map(frameFilter, vlog)

    intervals = []
    
    last_t0 = 0
    last_state = data[0][1]
    
    #offset_log = 149.629
    #offset_video = 5.41960191
    
    for d in data:
      if last_state != d[1]:
        t0 = d[0]*60# seconds in log - offset_log + offset_video # seconds in video
        
        if last_state == 'none':
          intervals.append({"type": "blank", "label":last_state, "begin": last_t0, "end": t0})
        else:
          intervals.append({"type": "button", "label":last_state, "begin": last_t0, "end": t0+3})
        
        last_t0 = t0
        last_state = d[1]
        
    #"./labels.json"
    with open(fileName, 'w') as outfile:
      json.dump({"intervals" : intervals}, outfile, indent=4, separators=(',', ': '))
    