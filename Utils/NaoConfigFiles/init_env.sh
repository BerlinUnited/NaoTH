#!/bin/bash

# copy function
copy(){
  local from="$1"
  local to="$2"
  local owner="$3"
  local rights="$4"
  
  if [ -f ${from} ]
  then
	  echo "copy ${from} to ${to}";
    
    # backup
    if [ -f ${to} ]
    then
      mv "${to}" "${to}.bak";
    fi
    
    # copy the file
	  cp -f ${from} ${to};
    
    #set correct rights
    echo "set owner ${owner}";
    chown ${owner}:${owner} ${to};
    echo "set rights ${rights}";
    chmod ${rights} ${to};
    
    return 0
  else
	  echo "missing file ${from}";
    return 1
  fi
}

# -----------  video driver -----------

# copy the video driver
kernel_name=$(uname -r)
video_driver_path=/lib/modules/$kernel_name/kernel/drivers/media/video/mt9m114.ko
copy ./kernel/drivers/media/video/mt9m114.ko $video_driver_path root 644

# -----------  set up the naoth running environment -----------

# NaoTH init script
copy ./etc/init.d/naoth /etc/init.d/naoth root 755

# NaoTH
copy ./usr/bin/naoth /usr/bin/naoth root 755

# brainwashinit
copy ./usr/bin/brainwash /usr/bin/brainwash root 755

# brainwash udev rule
copy ./etc/udev/rules.d/brainwashing.rules /etc/udev/rules.d/brainwashing.rules nao 644

# naoqi user autoload.ini
copy ./home/nao/naoqi/preferences/autoload.ini /home/nao/naoqi/preferences/autoload.ini nao 644

# naoqi system autoload.ini
copy ./etc/naoqi/autoload.ini /etc/naoqi/autoload.ini root 644

# copy ld.so.conf
copy ./etc/ld.so.conf /etc/ld.so.conf root 644

# create the local lib directory
if [ ! -d /home/nao/lib ]
then
    echo "setting library directory permissions";
    mkdir /home/nao/lib;
    if [ $? -ne 0 ]
    then
        echo "could not create /home/nao/lib"
    else
        chown -R nao:nao /home/nao/lib;
        chmod -R 755 /home/nao/lib;
    fi
fi

# add link to the Config directory
if [ ! -h /home/nao/Config ]
then
    echo "setting link to NaoTH Config directory";
    ln -s /home/nao/naoqi/Config /home/nao/Config;
fi

# -----------  system -----------

# Check and Update Runlevel Configuration for Non-Network Services
chown root:root ./checkRC.sh;
chmod 744 ./checkRC.sh;
./checkRC.sh "naoth=default netmount=disable lighttpd=disable naopathe=disable vsftpd=disable";

# allow everyone to shutdown
chmod +s /sbin/shutdown
chmod +s /sbin/reboot

# ----------- basic network configuration -----------

# LAN link
if [ ! -f /etc/init.d/net.eth0 ]
then
    echo "adding rc link for ethernet";
    ln -s /etc/init.d/net.lo /etc/init.d/net.eth0;
else
    echo "ethernet link exists";
fi

# WLAN link
if [ ! -f /etc/init.d/net.wlan0 ]
then
    echo "adding rc link for wireless";
    ln -s /etc/init.d/net.lo /etc/init.d/net.wlan0;
else
    echo "wireless link exists";
fi

#SSH Config
copy ./etc/ssh.conf/sshd_config /etc/ssh.conf/sshd_config root 644

#RC Config
copy ./etc/rc.conf /etc/rc.conf root 644

# Check and Update Runlevel Configuration for Network Services
chown root:root ./checkRC.sh;
chmod 744 ./checkRC.sh;
./checkRC.sh "connman=disable net.eth0=boot net.wlan0=boot";


# ----------- network -----------

# Check and Update Network Configuration
chown root:root ./init_net.sh;
chmod 744 ./init_net.sh;
./init_net.sh;


