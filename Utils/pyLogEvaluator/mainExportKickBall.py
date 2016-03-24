#!/usr/bin/python

import BehaviorParser
import json
import math
from matplotlib import pyplot
import numpy as np
import math2d as m2d

class Ball:
  def __init__(self):
    self.model = m2d.Vector2()
    self.model_velocity = m2d.Vector2()
    self.percept = m2d.Vector2()
    self.was_seen = False
    

def init():
    fileName = "D:\\RoboCup\\log\\kick-experiments\\2016-03-18-red-ball-lab\\Nao6026-short-left\\game.log"
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
  return option["state"].name == state and option["stateTime"] == 0

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
    if option_state(o, "sidekick", "sidekick_left_foot"):
      action = 'kick_right'
    elif option_state(o, "sidekick", "sidekick_right_foot"):
      action = 'kick_left'
    #elif option_state(o, "fast_forward_kick", "finished"):
    elif begin_option_state(o, "kick_short_with_approach_left", "done"):
      action = 'kick_short_left'
    elif "kick_with_foot" in o:
      action = 'kick_long'
   
    ball = Ball()
    ball.model.x = m["ball.x"]
    ball.model.y = m["ball.y"]
    ball.model_velocity.x = m["ball.speed.x"]
    ball.model_velocity.y = m["ball.speed.y"]
    ball.percept.x = m["ball.percept.x"]
    ball.percept.y = m["ball.percept.y"]
    ball.was_seen = m["ball.was_seen"]
   
    odometry = m2d.Pose2D()
    odometry.translation.x = frame["OdometryData"].pose.translation.x
    odometry.translation.y = frame["OdometryData"].pose.translation.y
    odometry.rotation = frame["OdometryData"].pose.rotation
   
    return [frame["FrameInfo"].time/(1000.0*60), action, ball, odometry]
    
  except KeyError:
    raise StopIteration
  

def run(vlog):

  # apply the filter
  #data = map(frameFilter, vlog)
  
  ball = []
  
  scan_ball = -1
  ball_label = "bum"
  origin = None
  
  pyplot.hold(True)
  for f in vlog:
    b = frameFilter(f)

    if scan_ball == -1 and b[1] != 'none':
      scan_ball = 400
      ball_label = b[1]
      origin = ~(b[3])
    
    if scan_ball > -1:
      scan_ball -= 1
      ballpos = origin*(b[3]*b[2].model)
      
      ball.append([ballpos.x, ballpos.y])

      if scan_ball == -1 or (scan_ball < 350 and b[2].model_velocity.abs() < 0.5):
        d = zip(*ball)
        pyplot.plot(d[0], d[1], label=ball_label)
        pyplot.plot(ballpos.x, ballpos.y, '*')
        ball = []
        scan_ball = -1
        
  pyplot.legend(loc='best')
  pyplot.show()
        