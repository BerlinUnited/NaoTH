#!/bin/bash
aplay /usr/share/naoqi/wav/chimes_short.wav

rm -rf ./backup
mkdir ./backup
cp -r /home/nao/naoqi/Config ./backup
cp -R /home/nao/bin ./backup

# stop naoqi and/or naoth
if [ -f "./deploy/home/nao/bin/naoth" ]; then
  /etc/init.d/naoth stop
fi

if [ -f "./deploy/home/nao/bin/libnaosmal.so" ]; then
  /etc/init.d/naoqi stop
fi

# remove files that will be copied
if [ -d "./deploy/home/nao/naoqi/Config" ]; then
  rm -rf /home/nao/Config/general
  rm -f /home/nao/Config/reachability_grid.dat
  rm -rf /home/nao/Config/robot_heads
  rm -rf /home/nao/Config/robots
  rm -rf /home/nao/Config/scheme
  rm -rf /home/nao/Config/scheme.cfg
  rm -rf /home/nao/Config/private
fi
if [ -f "./deploy/home/nao/bin/naoth" ]; then
  rm -f /home/nao/bin/naoth
fi
if [ -f "./deploy/home/nao/bin/libnaosmal.so" ]; then
  rm -f /home/nao/bin/libnaosmal.so
fi

# copy files that exist in deploy folder
if [ -d "./deploy/home/nao/naoqi/Config" ]; then
  sudo -u nao cp -nr ./deploy/home/nao/naoqi/Config/* /home/nao/Config/
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

