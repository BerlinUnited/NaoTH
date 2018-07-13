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

sleep 2

chown root:root ./checkRC.sh;
chmod 744 ./checkRC.sh;

# Set autoload.ini in /etc/naoqi to NaoTH
cat > /etc/naoqi/autoload.ini << EOL
# Here is the minimal list of modules loaded in NaoQi core process required by naoth. 

[core]
albase

[extra]
dcm_hal
alsystem

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
EOL

sleep 2
# enable naoth and disable the naoqi server
./checkRC.sh "naoth=default naopathe=disable nginx=disable"
# set the network to be configured by config
./checkRC.sh "connman=disable net.eth0=boot net.wlan0=boot"

reboot
