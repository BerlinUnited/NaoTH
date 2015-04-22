#!/bin/bash
aplay /usr/share/naoqi/wav/chimes_short.wav

# stop naoqi and/or naoth if they are to replace
if [ -f "./deploy/home/nao/bin/naoth" ]; then
  /etc/init.d/naoth stop
fi
if [ -f "./deploy/home/nao/bin/libnaosmal.so" ]; then
  /etc/init.d/naoqi stop
fi

# make sure wifi is running
if [ -f "./deploy/disablewifi" ]; then
  rc-update del net.wlan0 boot 
  /etc/init.d/net.wlan0 stop
fi
if [ -f "./deploy/enablewifi" ]; then
   rc-update add net.wlan0 boot
  /etc/init.d/net.wlan0 start
fi

# backup the stuff from the robot
rm -rf ./backup
sudo -u nao mkdir ./backup
sudo -u nao cp -r /home/nao/naoqi/Config ./backup
sudo -u nao cp -r /home/nao/bin ./backup

# remove files that will be copied and copy the new ones
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
  
  sudo -u nao cp -r ./deploy/home/nao/naoqi/Config/* /home/nao/Config/
fi

# copy binaries and start naoqi/naoth again
if [ -f "./deploy/home/nao/bin/libnaosmal.so" ]; then
  rm -f /home/nao/bin/libnaosmal.so
  sudo -u nao cp ./deploy/home/nao/bin/libnaosmal.so /home/nao/bin/libnaosmal.so
  /etc/init.d/naoqi start
fi

if [ -f "./deploy/home/nao/bin/naoth" ]; then
  rm -f /home/nao/bin/naoth
  sudo -u nao cp ./deploy/home/nao/bin/naoth /home/nao/bin/naoth
  sudo -u nao chmod 755 /home/nao/bin/naoth
  /etc/init.d/naoth start
fi

echo "DONE"


