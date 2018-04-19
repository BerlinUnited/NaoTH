# -*- encoding: UTF-8 -*- 

import sys
import time

from naoqi import ALProxy

def runBehavior(managerProxy, behaviorName):
  # Check that the behavior exists.
  if (managerProxy.isBehaviorInstalled(behaviorName)):

    # Check that it is not already running.
    if (not managerProxy.isBehaviorRunning(behaviorName)):
      # Launch behavior. This is a blocking call, use post if you do not
      # want to wait for the behavior to finish.
      managerProxy.post.runBehavior(behaviorName)
    else:
      print "Behavior is already running."
      return True

  else:
    print "Behavior not found."
    return False
  
  return False

def runDefaultBehaviors():

  try:
    managerProxy = ALProxy("ALBehaviorManager", "localhost", 9559)
  except:
    return False
  
  names = managerProxy.getDefaultBehaviors()
  print "Default behaviors:"
  print names
  
  result = True
  for name in names:
    if not runBehavior(managerProxy, name):
      result = False
    
  time.sleep(5)
  names = managerProxy.getRunningBehaviors()
  print "Running behaviors:"
  print names
  return result
  

if __name__ == "__main__":

  while not runDefaultBehaviors():
    time.sleep(5)