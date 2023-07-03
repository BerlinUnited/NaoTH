#!/usr/bin/env python2
# -*- coding: utf-8 -*-
import socket
import msgpack
import time
import math

class Robot :
  def __init__ (self):
    self.socket = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    self.socket.connect( "/tmp/robocup" )
    self.joints = [
      "HeadYaw" ,
      "HeadPitch" ,
      "LShoulderPitch" ,
      "LShoulderRoll" ,
      "LElbowYaw" ,
      "LElbowRoll" ,
      "LWristYaw" ,
      "LHipYawPitch" ,
      "LHipRoll" ,
      "LHipPitch" ,
      "LKneePitch" ,
      "LAnklePitch" ,
      "LAnkleRoll" ,
      "RHipRoll" ,
      "RHipPitch" ,
      "RKneePitch" ,
      "RAnklePitch" ,
      "RAnkleRoll" ,
      "RShoulderPitch" ,
      "RShoulderRoll" ,
      "RElbowYaw" ,
      "RElbowRoll" ,
      "RWristYaw" ,
      "LHand" ,
      "RHand"
    ]
    self.sonars = [
      "Left" ,
      "Right" ,
    ]
    self.touch = [
      "ChestBoard/Button" ,
      "Head/Touch/Front" ,
      "Head/Touch/Middle" ,
      "Head/Touch/Rear" ,
      "LFoot/Bumper/Left" ,
      "LFoot/Bumper/Right" ,
      "LHand/Touch/Back" ,
      "LHand/Touch/Left" ,
      "LHand/Touch/Right" ,
      "RFoot/Bumper/Left" ,
      "RFoot/Bumper/Right" ,
      "RHand/Touch/Back" ,
      "RHand/Touch/Left" ,
      "RHand/Touch/Right" ,
    ]
    self.LEar = [
      "0" ,
      "36" ,
      "72" ,
      "108" ,
      "144" ,
      "180" ,
      "216" ,
      "252" ,
      "288" ,
      "324"
    ]
    self.REar = [
      "324" ,
      "288" ,
      "252" ,
      "216" ,
      "180" ,
      "144" ,
      "108" ,
      "72" ,
      "36" ,
      "0"
    ]
    self.actuators = {
      'Position' : self.joints,
      'Stiffness' : self.joints,
      'Chest' : [ 'Red' , 'Green' , 'Blue' ],
      'Sonar' : self.sonars,
      'LEar' : self.LEar,
      'REar' : self.REar
    }
    self.commands = {
      'Position' : [ 0.0 ] * 25 ,
      'Stiffness' : [ 0.0 ] * 25 ,
      'Chest' : [ 0.0 ] * 3 ,
      'Sonar' : [ True , True ] ,
      'LEar' : [ 0.0 ] * 10 ,
      'REar' : [ 0.0 ] * 10
    }
    
  def read (self):
    stream = self.socket.recv( 896 )
    upacker = msgpack.unpackb(stream, raw=False)
    return upacker
    
  def command (self, category, device, value):
    self.commands[category][self.actuators[category].index(device)] = value

  def send (self):
    stream = msgpack.packb(self.commands)
    self.socket.send(stream)
    
  def close (self):
    self.socket.close()
    

def main ():
  robot = Robot()
  try :
    robot.command( "Position" , "HeadYaw" , 0.0 )
    robot.command( "Position" , "HeadPitch" , 0.0 )
    robot.command( "Stiffness" , "HeadYaw" , 1.00 )
    robot.command( "Stiffness" , "HeadPitch" , 1.00 )
    ear_completion = 0
    head_pitch = 0.0

    while True :
      data = robot.read()
      positions_value = data[ "Position" ]
      positions = {}
      for index,name in enumerate (robot.joints):
        positions[name] = positions_value[index]
      sonars_value = data[ "Sonar" ]
      distances = {}
      for index,name in enumerate (robot.sonars):
        distances[name] = sonars_value[index]
      print(distances)
      touch_value = data[ "Touch" ]
      touch = {}
      for index,name in enumerate (robot.touch):
        touch[name] = touch_value[index]
      if touch[ 'Head/Touch/Front' ] == True :
        if ear_completion <= 9.0 :
          ear_completion += 0.1
        if head_pitch <= 1.0 :
          head_pitch += 0.01
      elif touch[ 'Head/Touch/Rear' ] == True :
        if ear_completion > 0.0 :
          ear_completion -= 0.1
        if head_pitch >= -1.0 :
          head_pitch -= 0.01
          
      robot.commands[ 'LEar' ] = [ 1.0 if n <= int (ear_completion) else 0.0 for n in range ( 0 , 10 )]
      robot.commands[ 'LEar' ][ int (ear_completion)] = float (ear_completion - int (ear_completion))
      robot.commands[ 'REar' ] = [ 1.0 if n <= int (ear_completion) else 0.0 for n in range ( 0 , 10 )]
      robot.commands[ 'REar' ][ int (ear_completion)] = float (ear_completion - int (ear_completion))
      robot.command( "Chest" , "Red" , abs (math.sin( 2 *time.time())))
      robot.command( "Chest" , "Blue" , abs (math.sin( 2 *time.time())))
      robot.command( "Position" , "HeadPitch" , head_pitch)
      robot.send()
  except KeyboardInterrupt :
    print("Exit")
  finally :
    robot.close()
  
if __name__ == "__main__" :
  main()
