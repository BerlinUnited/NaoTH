#!/usr/bin/env python3
# -*- coding: utf8 -*-

import tempfile
import os
import threading
from time import sleep
import time

from daemonize import Daemonize
from display import Display
from goal_sensor import GoalSensor

from sender import TeamCommSender
from receiver import TeamCommReceiver


def main():

  print("run goal count")
  display = Display()
  goalSensor = GoalSensor()
  
  receiver = TeamCommReceiver()
  receiver.start()
  
  sender = TeamCommSender('10.0.4.255')
  sender.start()
  
  last_time = receiver.time_playing
  try:
  
    #while not goalSensor.update.wait():
      #display.set_score(random.randint(0, 99), random.randint(0, 99))
      #sleep(1)
      #print ("set score: {}:{}".format(goal1, goal2))
      
    while True:
      # a new game started
      if last_time == 0 and receiver.time_playing > 0:
        goalSensor.reset()
    
      if receiver.time_playing > 0:
        display.setValues(goalSensor.goal1, goalSensor.goal2, receiver.time_playing)
        sender.set_score(goalSensor.goal1, goalSensor.goal2, receiver.time_playing)
        sleep(0.5)
      else:
        #display.showRandomImage()
        display.showNextImage()
        sleep(2)
        
      last_time = receiver.time_playing
      
      
  except (KeyboardInterrupt, SystemExit):
    display.end()
    print("Interrupted or Exit")
    print("")  # intentionally print empty line
    goalSensor.close()
    receiver.stop()
    sender.stop()
    
  goalSensor.close()
  #receiver.stop()
  #sender.stop()
    

if __name__ == '__main__':
    print ("Starting Goal Count")
    
    # define vars
    tempdir = tempfile.gettempdir()
    name = 'pyGoalCount'
    # check for existing lock file and running process
    lock_file = os.path.join(tempdir, name + '.lock')

    #main()
    daemon = Daemonize(app=name, pid=lock_file, action=main, foreground=True)
    daemon.start()
