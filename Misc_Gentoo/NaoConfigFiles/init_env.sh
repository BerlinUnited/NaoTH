#!/bin/bash

# Check and Update Runlevel Configuration
chown root:root ./init_net.sh
chmod 744 ./init_net.sh
./init_net.sh

# Video
if [ ! -f /etc/init.d/set_dev_video ]  
then
    if [ -f ./etc/init.d/set_dev_video ]
    then
	echo "adding rc script for /dev/video0 to /dev/video mapping"
	cp ./etc/init.d/set_dev_video /etc/init.d/set_dev_video
	chown root:root /etc/init.d/set_dev_video
	chmod 755 /etc/init.d/set_dev_video
    else
	echo "init script set_dev_video is missing"
    fi
else
    echo "set_dev_video exists"
fi

# Cognition Common
if [ ! -f /etc/init.d/cognition-common ]  
then
    if [ -f ./etc/init.d/cognition-common ]
    then
	echo "adding script cognition-common"
	cp -f ./etc/init.d/cognition-common /etc/init.d/cognition-common
	chown root:root /etc/init.d/cognition-common
	chmod 755 /etc/init.d/cognition-common
    else
	echo "init script cognition-common is missing"
    fi
else
    echo "cognition-common exists"
fi

# NaoTH
if [ ! -f /etc/init.d/naoth ]  
then
    if [ -f ./etc/init.d/naoth ]
    then
	echo "adding rc script naoth"
	cp -f ./etc/init.d/naoth /etc/init.d/naoth
	chown root:root /etc/init.d/naoth
	chmod 755 /etc/init.d/naoth
    else
	echo "init script naoth is missing"
    fi
else
    echo "naoth exists"
fi



# Check and Update Runlevel Configuration for Non-Network Services
./checkRC.sh "set_dev_video=default connman=disable naoth=default"






