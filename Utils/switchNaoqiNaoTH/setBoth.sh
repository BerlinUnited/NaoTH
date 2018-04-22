#!/bin/bash
#
# Copy this file to the robot and execute there to change default Naoqi to NaoTH
#
#check if root first
if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root"
   exit 1
fi

# set volume to 88%
sudo -u nao pactl set-sink-mute 0 false
sudo -u nao pactl set-sink-volume 0 88%

# play initial sound
sudo -u nao /usr/bin/paplay /home/nao/naoqi/Media/usb_start.wav

# nao stop
/etc/init.d/naoqi stop
naoth stop

sleep 2

chown root:root ./checkRC.sh;
chmod 744 ./checkRC.sh;

# set naoth to default and enable the naoqi server
./checkRC.sh "naoth=default naopathe=boot nginx=boot"

# set the network to be configured by config
./checkRC.sh "connman=disable net.eth0=boot net.wlan0=boot"

# Set autoload.ini in /etc/naoqi to Naoqi
cat > /etc/naoqi/autoload.ini << EOL
# Here is the list of modules loaded in NaoQi core process.

# You should not edit this file.

# If you need to load your module at startup, create a file
# in /home/nao/naoqi/preferences/autoload.ini looking like:

#  [user]
#  /the/full/path/to/your/libyourmodule.so
#
# Note: to use urbi, you should use:
#
#  [user]
#  /usr/lib/naoqi/liburbistarter.so
#
[core]
albase
preferencemanager
framemanager
logmanager
alservicemanager
aldebug

[extra]
notificationmanager
dcm_hal
expressiveness
alresourcemanager
robotmodel
aldiagnosis
sensors
albodytemperature
motion
touch
robotposture
#redballtracker
#facetracker
#motionrecorder
#leds
#alworldrepresentation
#alvideodevice # need to be disabled because if active naoth can't access the cameras and an assert will fail
#colorblobdetection
#redballdetection
#facedetection
#visionrecognition
#landmarkdetection
#darknessdetection
#backlightingdetection
#photocapture
#videorecorder
#alvisualcompass
#visualspacehistory
alsystem
packagemanager
alusersession

# modularity and filters
#modularityperception
#audiofilterloader

audiodevice
#audiorecorder
audioout
#speechrecognition
#voiceemotionanalysis
albattery
#alchestbutton
#mecalogger
pythonbridge
launcher
#allaser
behaviormanager
animatedspeech
memorywatcher
#navigation
albonjour
#infrared
altelepathe
#altracker
#allocalization
#alpanoramacompass
robothealthmonitor
notificationreader

# vision modules using modularity
#segmentation3D
#barcodereader
#movementdetection
#peopleperception
#engagementzones
#sittingpeopledetection
#gazeanalysis
#wavingdetection
#facecharacteristics
#objectdetection
#closeobjectdetection
#fastpersontracking
#findpersonhead

# audition modules using modularity
#sounddetection
#soundlocalization

#basicawareness
#autonomousblinking
#autonomousmoves
#autonomouslife
#dialog
EOL

# Set autoload.ini in home to NaoTH
cat > /home/nao/naoqi/preferences/autoload.ini << EOL
# autoload.ini
#
# Use this file to list the cross-compiled modules that you wish to load.
# You must specify the full path to the module, python module or program.

[user]
#the/full/path/to/your/liblibraryname.so  # load liblibraryname.so
/home/nao/bin/libnaosmal.so

[python]
#the/full/path/to/your/python_module.py   # load python_module.py

[program]
#the/full/path/to/your/program            # load program
/home/nao/bin/runDefaultBehaviors
EOL

# enable deactivation possibility of fall manager
cat > /home/nao/.config/naoqi/ALMotion.xml << EOL
<?xml version="1.0" encoding="UTF-8" ?>
<ModulePreference name="aldebaran-robotics.com@ALMotion" xmlns="http://www.aldebaran-robotics.com/ns/ALPreference" schemaLocation="ModulePreference.xsd">
    <Preference name="ENABLE_FOOT_CONTACT_PROTECTION" description="If true the walk task will be killed or prevented from launching if no foot contact(footContact)" value="true" type="bool" />
    <Preference name="ENABLE_STIFFNESS_PROTECTION" description="If true the walk task will be killed or prevented from launching if one joint of the legs has a Stiffness equal or less than 0.6" value="true" type="bool" />
    <Preference name="DIAGNOSTIC_IS_OK" description="If true the diagnostic of the robot is ok or have never been called" value="true" type="bool" />
    <Preference name="ENABLE_DEACTIVATION_OF_FALL_MANAGER" description="If true the deactivation of Fall Manager and other critical safety reflexes is allowed" value="true" type="bool" />
</ModulePreference>
EOL

cat > /home/nao/bin/runDefaultBehaviors << EOL
#!/usr/bin/python 
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
EOL

chmod 755 /home/nao/bin/runDefaultBehaviors;

sleep 2

# restart the robot
reboot
