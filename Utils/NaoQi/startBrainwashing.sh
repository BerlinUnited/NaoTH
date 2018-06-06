#!/bin/bash

# set volume to 88%
sudo -u nao pactl set-sink-mute 0 false
sudo -u nao pactl set-sink-volume 0 88%

# play initial sound
sudo -u nao /usr/bin/paplay /home/nao/naoqi/Media/usb_start.wav

behavior_folder="/home/nao/.local/share/PackageManager/apps/naoqith_switch_naoqi_backend_behavior-3ab43e"

if [ -d $behavior_folder ]; then
    rm -rf $behavior_folder
fi

sudo -u nao mkdir $behavior_folder
sudo -u nao cp -r ./naoQiTH_switch_naoqi_backend_behavior $behavior_folder

default_config="/home/nao/.config/naoqi/DefaultBehaviors.xml"

if [ -f $default_config ]; then
    rm -f $default_config
fi

sudo -u nao cp ./DefaultBehaviors.xml $default_config

bin_folder="/home/nao/bin"

if [ -f $bin_folder ]; then
    rm -f $bin_folder
fi

sudo -u nao cp ./runDefaultBehaviors $bin_folder
sudo -u nao chmod +x $bin_folder'/runDefaultBehaviors'

/etc/init.d/naoqi restart

echo "Done"
