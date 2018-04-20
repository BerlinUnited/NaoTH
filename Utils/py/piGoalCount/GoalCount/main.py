#!/usr/bin/env python3
# -*- coding: utf8 -*-

import tempfile
from daemonize import Daemonize
from display import Display
import os
from time import sleep
import random

import RPi.GPIO as GPIO
import threading

GPIO.setmode(GPIO.BCM)
 
#set GPIO Pins
GPIO_INPUT_GOAL1 = 23
GPIO_INPUT_GOAL2 = 24
 
#set GPIO direction (IN / OUT)
GPIO.setup([GPIO_INPUT_GOAL1, GPIO_INPUT_GOAL2], GPIO.IN, pull_up_down=GPIO.PUD_DOWN)

update = threading.Event()
goal1 = 0
time1 = 0
goal2 = 0
time2 = 0

def updateGoal1(pin):
  global update
  goal1 += 1
  update.set()
  
def updateGoal2(pin):
  global update
  goal2 += 1
  update.set()

def main():
  global update
  global goal1
  global goal2
  
  print("run goal count")
  display = Display()
  
  GPIO.add_event_detect(GPIO_INPUT_GOAL1, GPIO.RISING, updateGoal1)
  GPIO.add_event_detect(GPIO_INPUT_GOAL2, GPIO.RISING, updateGoal2)
  
  while not update.wait():
    #display.set_score(random.randint(0, 99), random.randint(0, 99))
    #sleep(1)
    display.set_score(goal1, goal2)
    

if __name__ == '__main__':
    
    # define vars
    tempdir = tempfile.gettempdir()
    name = 'pyGoalCount'
    # check for existing lock file and running process
    lock_file = os.path.join(tempdir, name + '.lock')

    daemon = Daemonize(app=name, pid=lock_file, action=main, foreground=True)
    daemon.start()
