#!/usr/bin/env python3
# -*- coding: utf8 -*-

import tempfile
from daemonize import Daemonize
from display import Display
import os
from time import sleep
import random

def main():
  print("run goal count")
  display = Display()
  
  while True:
    display.set_score(random.randint(0, 99), random.randint(0, 99))
    sleep(1)
    
if __name__ == '__main__':
    
    # define vars
    tempdir = tempfile.gettempdir()
    name = 'pyGoalCount'
    # check for existing lock file and running process
    lock_file = os.path.join(tempdir, name + '.lock')

    daemon = Daemonize(app=name, pid=lock_file, action=main, foreground=True)
    daemon.start()
