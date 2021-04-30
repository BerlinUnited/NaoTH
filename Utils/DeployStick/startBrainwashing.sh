#!/bin/bash

# print all commands just in case
set -x #echo on

echo "START DEPLOY"

# set volume to 88%
su nao -c "/usr/bin/pactl set-sink-mute 0 false"
su nao -c "/usr/bin/pactl set-sink-volume 0 88%"
# also set the recording volume
# 1. set in simple mode with alsa mixer to make sure it is in sync for all channels
su nao -c "/usr/bin/amixer sset 'Capture',0 90%"
# 2. set with pulseaudio (now both channels are set) to make sure the changes are persistent
su nao -c "/usr/bin/pactl set-source-mute 1 false"
su nao -c "/usr/bin/pactl set-source-volume 1 90%"


# play initial sound
su nao -c "/usr/bin/paplay /home/nao/naoqi/Media/usb_start.wav"

# we're replacing something necessary - stop naoth in any case
naoth stop

# backup the stuff from the robot
echo "[deploy] backup the stuff on the robot"
rm -rf ./backup
su nao -c "mkdir ./backup"
su nao -c "cp -r /home/nao/naoqi/Config ./backup"
su nao -c "cp -rv /home/nao/bin ./backup"

# remove files that will be copied and copy the new ones
echo "[deploy] clean and copy new files"
if [ -d "./deploy/home/nao/naoqi/Config" ]; then
  rm -rf /home/nao/Config/general
  rm -rf /home/nao/Config/platform
  rm -rf /home/nao/Config/robots
  rm -rf /home/nao/Config/robot_heads
  rm -rf /home/nao/Config/scheme
  rm -rf /home/nao/Config/private
  rm -f /home/nao/Config/scheme.cfg
  rm -f /home/nao/Config/behavior-ic.dat
  rm -f /home/nao/Config/reachability_grid.dat
  
  su nao -c "cp -r ./deploy/home/nao/naoqi/Config/* /home/nao/Config/"
fi

# run only on Nao V3 to V5
if [ ! -f "/opt/aldebaran/bin/lola" ] && [ ! -f "/usr/bin/lola" ]; then
  # md5 hashes from the autoload.ini in the repository and the current autoload.ini on the robot
  hash1='fd2a49e4ad1e6583be697444bbd8e746'
  hash2=`md5sum /etc/naoqi/autoload.ini | awk '{print $1}'`

  if [[ $hash1 != $hash2 ]]; then
    # The MD5 sum didn't match
    su nao -c "/usr/bin/paplay /home/nao/naoqi/Media/modified_autoload_ini.wav"
  fi

  # replace libNaoSMAL if avaliable
  if [ -f "./deploy/home/nao/bin/libnaosmal.so" ]; then
    echo "[deploy] replace libnaosmal"
    # NOTE: command 'nao' on V5 is an alias which is not avaliable at the time when this script is executed,
    #       so we use the command directly instead.
    #  alias nao='sudo /etc/init.d/naoqi'
    /etc/init.d/naoqi stop
    rm -f /home/nao/bin/libnaosmal.so
    su nao -c "cp ./deploy/home/nao/bin/libnaosmal.so /home/nao/bin/libnaosmal.so"
    /etc/init.d/naoqi start
  fi
else
  # copy binaries and start naoqi/naoth again
  if [ -f "./deploy/home/nao/bin/lola_adaptor" ]; then
    echo "[deploy] replace lola_adaptor"
    lola_adaptor stop
    rm -f /home/nao/bin/lola_adaptor
    su nao -c "cp ./deploy/home/nao/bin/lola_adaptor /home/nao/bin/lola_adaptor"
    chmod 755 /home/nao/bin/lola_adaptor
  fi

fi

if [ -f "./deploy/home/nao/bin/naoth" ]; then
  echo "[deploy] replace naoth"
  rm -f /home/nao/bin/naoth
  su nao -c "cp ./deploy/home/nao/bin/naoth /home/nao/bin/naoth"
  chmod 755 /home/nao/bin/naoth
fi

naoth start

# wait until the binary is started so we can collect its output
sleep 4

# collect the last 1024 lines of the output
tail -n 1024 /var/log/messages > ./braindump.txt

echo "DONE"
