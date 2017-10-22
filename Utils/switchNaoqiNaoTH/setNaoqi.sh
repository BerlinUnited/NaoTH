#!/bin/bash
#
# Copy this file to the robot and execute there to change NaoTH to default Naoqi
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

naoth stop
/etc/init.d/naoqi stop

sleep 2

# hier passiert die eigentliche arbeit
chown root:root ./checkRC.sh;
chmod 744 ./checkRC.sh;

# disable naoth and enable the naoqi server
./checkRC.sh "naoth=disable naopathe=boot nginx=boot"

# set the network to be configured by connman
./checkRC.sh "connman=boot net.eth0=disable net.wlan0=disable"

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
redballtracker
facetracker
motionrecorder
leds
alworldrepresentation
alvideodevice
colorblobdetection
redballdetection
facedetection
visionrecognition
landmarkdetection
darknessdetection
backlightingdetection
photocapture
videorecorder
alvisualcompass
visualspacehistory
alsystem
packagemanager
alusersession

# modularity and filters
modularityperception
audiofilterloader

audiodevice
audiorecorder
audioout
speechrecognition
voiceemotionanalysis
albattery
alchestbutton
mecalogger
pythonbridge
launcher
allaser
behaviormanager
animatedspeech
memorywatcher
navigation
albonjour
infrared
altelepathe
altracker
allocalization
alpanoramacompass
robothealthmonitor
notificationreader

# vision modules using modularity
segmentation3D
barcodereader
movementdetection
peopleperception
engagementzones
sittingpeopledetection
gazeanalysis
wavingdetection
facecharacteristics
#objectdetection
closeobjectdetection
fastpersontracking
findpersonhead

# audition modules using modularity
sounddetection
soundlocalization

basicawareness
#autonomousblinking
#autonomousmoves
#autonomouslife
dialog
EOL


# Set autoload.ini in home to Naoqi
cat > /home/nao/naoqi/preferences/autoload.ini << EOL
# autoload.ini
#
# Use this file to list the cross-compiled modules that you wish to load.
# You must specify the full path to the module, python module or program.

[user]
#the/full/path/to/your/liblibraryname.so  # load liblibraryname.so

[python]
#the/full/path/to/your/python_module.py   # load python_module.py

[program]
#the/full/path/to/your/program            # load program
EOL

sleep 2

# start the robot
reboot
