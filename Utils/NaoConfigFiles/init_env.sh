#!/bin/bash
# Video
if [ ! -f /etc/init.d/set_dev_video ]  
then
    if [ -f ./etc/init.d/set_dev_video ]
    then
	echo "adding rc script for /dev/video0 to /dev/video mapping";
	cp ./etc/init.d/set_dev_video /etc/init.d/set_dev_video;
	chown root:root /etc/init.d/set_dev_video;
	chmod 755 /etc/init.d/set_dev_video;
    else
	echo "init script set_dev_video is missing";
    fi
else
    echo "set_dev_video exists";
fi

# copy the video driver
kernel_name=$(uname -r)
video_driver_path=/lib/modules/$kernel_name/kernel/drivers/media/video/mt9m114.ko
if [ -f video_driver_path ]  
then
  mv $video_driver_path $video_driver_path.bak
fi
cp ./kernel/drivers/media/video/mt9m114.ko $video_driver_path

# Cognition Common
if [ ! -f /etc/init.d/cognition-common ]  
then
    if [ -f ./etc/init.d/cognition-common ]
    then
	echo "adding script cognition-common";
	cp -f ./etc/init.d/cognition-common /etc/init.d/cognition-common;
	chown root:root /etc/init.d/cognition-common;
	chmod 755 /etc/init.d/cognition-common;
    else
	echo "init script cognition-common is missing";
    fi
else
    echo "cognition-common exists";
fi

# NaoTH
if [ ! -f /etc/init.d/naoth ]  
then
    if [ -f ./etc/init.d/naoth ]
    then
	echo "adding rc script naoth";
	cp -f ./etc/init.d/naoth /etc/init.d/naoth;
	chown root:root /etc/init.d/naoth;
	chmod 755 /etc/init.d/naoth;
    else
	echo "init script naoth is missing";
    fi
else
    echo "naoth exists";
fi

# NaoTH
if [ -f ./naoth ]
then
    echo "adding start script naoth";
    cp -f ./usr/bin/naoth /usr/bin/naoth;
    chown root:root /usr/bin/naoth;
    chmod 755 /usr/bin/naoth;
else
    echo "start script naoth is missing";
fi

# brainwashinit
if [ -f ./bin/brainwash ]
then
    echo "adding start script brainwash";
    cp -f ./bin/brainwash /usr/bin/brainwash;
    chown root:root /usr/bin/brainwash;
    chmod 755 /usr/bin/brainwash;
else
    echo "brainwash script is missing";
fi

# brainwash udev rule
if [ -f ./etc/udev/rules.d/brainwashing.rules ]
then
    echo "setting udev brainwashing.rules";
    cp -f ./etc/udev/rules.d/brainwashing.rules /etc/udev/rules.d/brainwashing.rules;
    chown root:root /etc/udev/rules.d/brainwashing.rules;
    chmod 644 /etc/udev/rules.d/brainwashing.rules;
else
    echo "udev brainwashing.rules is missing";
fi


# naoqi user autoload.ini
if [ -f ./autoload.ini ]
then
    echo "setting naoqi user autoload.ini";
    cp -f ./autoload.ini /home/nao/naoqi/preferences/autoload.ini;
    chown nao:nao /home/nao/naoqi/preferences/autoload.ini;
    chmod 644 /home/nao/naoqi/preferences/autoload.ini;
else
    echo "naoqi user autoload.ini is missing";
fi

# naoqi system autoload.ini
if [ -f ./etc/naoqi/autoload.ini ]
then
    echo "setting naoqi system autoload.ini";
    cp -f ./etc/naoqi/autoload.ini /etc/naoqi/autoload.ini;
    chown root:root /etc/naoqi/autoload.ini;
    chmod 644 /etc/naoqi/autoload.ini;
else
    echo "naoqi system autoload.ini is missing";
fi

# check for link to our local lib directory
if [ ! -d /home/nao/lib ]
then
    echo "setting library directory permissions";
    mkdir /home/nao/lib;
fi

if [ -d /home/nao/lib ]
then
    chown -R nao:nao /home/nao/lib;
    chmod -R 755 /home/nao/lib;
fi

if [ ! -h /home/nao/Config ]
then
    echo "setting link to NaoTH Config directory";
    ln -s /home/nao/naoqi/Config /home/nao/Config;
fi

# copy naoth-profile.sh
if [ -f ./etc/ld.so.conf ]
then
    echo "copy ld.so.conf.d/naoth.conf";
    cp -f ./etc/ld.so.conf /etc/ld.so.conf;
    chown root:root /etc/ld.so.conf;
    chmod 755 /etc/ld.so.conf;
else
    echo "ld.so.conf.d/naoth.conf is missing";
fi

# register avahi services
if [ -f ./etc/avahi/services/naoth.service ]
then
    echo "setting avahi service for naoth";
    cp -f ./etc/avahi/services/naoth.service /etc/avahi/services/naoth.service;
    chown root:root /etc/avahi/services/naoth.service;
    chmod 644 /etc/avahi/services/naoth.service;
else
    echo "avahi service config file is missing";
fi

# Check and Update Runlevel Configuration for Non-Network Services
chown root:root ./checkRC.sh;
chmod 744 ./checkRC.sh;
./checkRC.sh "set_dev_video=default naoth=default netmount=disable lighttpd=disable naopathe=disable vsftpd=disable";

# Check and Update Network Configuration
chown root:root ./init_net.sh;
chmod 744 ./init_net.sh;
./init_net.sh;

# check for link to librt.so
if [ ! -h /home/nao/lib/librt.so ]
then
    ln -s /usr/librt.so.1 /home/nao/lib/librt.so;
fi

ldconfig;

# allow everyone to shutdown
chmod +s /sbin/shutdown
chmod +s /sbin/reboot


