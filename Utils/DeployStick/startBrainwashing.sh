#!/bin/bash
aplay /usr/share/naoqi/wav/chimes_short.wav

# stop naoqi and/or naoth if they are to replace
if [ -f "./deploy/home/nao/bin/naoth" ]; then
  /etc/init.d/naoth stop
fi
if [ -f "./deploy/home/nao/bin/libnaosmal.so" ]; then
  /etc/init.d/naoqi stop
fi

# backup the stuff from the robot
rm -rf ./backup
mkdir ./backup
cp -r /home/nao/naoqi/Config ./backup
cp -r /home/nao/bin ./backup

# remove files that will be copied
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
fi
if [ -f "./deploy/home/nao/bin/naoth" ]; then
  rm -f /home/nao/bin/naoth
fi
if [ -f "./deploy/home/nao/bin/libnaosmal.so" ]; then
  rm -f /home/nao/bin/libnaosmal.so
fi

# copy files that exist in deploy folder
if [ -d "./deploy/home/nao/naoqi/Config" ]; then
  sudo -u nao cp -r ./deploy/home/nao/naoqi/Config/* /home/nao/Config/
fi

if [ -d "./deploy/home/nao/bin" ]; then
  sudo -u nao cp ./deploy/home/nao/bin/* /home/nao/bin/
fi

# start naoqi/naoth again
if [ -f "./deploy/home/nao/bin/libnaosmal.so" ]; then
  /etc/init.d/naoqi start
fi
if [ -f "./deploy/home/nao/bin/naoth" ]; then
  /etc/init.d/naoth start
fi

