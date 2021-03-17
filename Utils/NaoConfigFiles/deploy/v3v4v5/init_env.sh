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
			if [ ! -f "${to}.bak" ]
			then
				mv "${to}" "${to}.bak";
			fi
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
su nao -c "/usr/bin/paplay ../home/nao/naoqi/Media/usb_start.wav"

# -----------  video driver -----------

# copy the video driver
kernel_name=$(uname -r)
video_driver_path=/lib/modules/$kernel_name/kernel/drivers/media/video/mt9m114.ko
copy ./kernel/drivers/media/video/mt9m114.ko $video_driver_path root 644

# ----------- make boot faster ---------

# brainwashinit
copy ./etc/init.d/checkpart-dummy /etc/init.d/checkpart-dummy root 755

# -----------  set up the naoth running environment -----------

# NaoTH init script
copy ./etc/init.d/naoth /etc/init.d/naoth root 755

# Needed by NaoTH init script
copy ./etc/init.d/cognition-common /etc/init.d/cognition-common root 755

# NaoTH
copy ./usr/bin/naoth /usr/bin/naoth root 755

# brainwashinit
copy ./usr/bin/brainwash /usr/bin/brainwash root 755

# brainwash udev rule
copy ./etc/udev/rules.d/brainwashing.rules /etc/udev/rules.d/brainwashing.rules nao 644
copy ./etc/udev/rules.d/disable-wifi-power-save.rules /etc/udev/rules.d/disable-wifi-power-save.rules nao 644

# naoqi user autoload.ini
copy ./home/nao/naoqi/preferences/autoload.ini /home/nao/naoqi/preferences/autoload.ini nao 644

# naoqi system autoload.ini
copy ./etc/naoqi/autoload.ini /etc/naoqi/autoload.ini root 644

# copy ld.so.conf
copy ./etc/ld.so.conf /etc/ld.so.conf root 644

# copy new fstab
copy ./etc/fstab /etc/fstab root 644

# add syslogger config
copy ./etc/syslog.conf /etc/syslog.conf root 644

# update the system wide libstdc++
if [ -e ../home/nao/lib/libstdc++.so ]
then
    mkdir /usr/lib/backup_libst++
    cp /usr/lib/libstdc++.* /usr/lib/backup_libst++/
    cp  ../home/nao/lib/libstdc++.* /usr/lib/
fi

# create the local lib directory
if [ ! -d /home/nao/lib ]
then
    mkdir /home/nao/lib;
    if [ $? -ne 0 ]
    then
        echo "could not create /home/nao/lib"
    else
        chown nao:nao /home/nao/lib;
        chmod 744 /home/nao/lib;
    fi
fi

# create the local bin directory
if [ ! -d /home/nao/bin ]
then
    mkdir /home/nao/bin;
    if [ $? -ne 0 ]
    then
        echo "could not create /home/nao/bin"
    else
       chown nao:nao /home/nao/bin;
       chmod 755 /home/nao/bin;
    fi
fi

# create the local Config directory
if [ ! -d /home/nao/naoqi/Config ]
then
    mkdir -p /home/nao/naoqi/Config;
    if [ $? -ne 0 ]
    then
        echo "could not create /home/nao/naoqi/Config"
    else
        chown nao:nao /home/nao/naoqi/Config;
        chmod 744 /home/nao/naoqi/Config;
    fi
fi

# add link to the Config directory
if [ ! -h /home/nao/Config ]
then
    echo "setting link to NaoTH Config directory";
    ln -s /home/nao/naoqi/Config /home/nao/Config;
fi

# create the local Media directory
if [ ! -d /home/nao/naoqi/Media ]
then
    mkdir -p /home/nao/naoqi/Media;
    if [ $? -ne 0 ]
    then
        echo "could not create /home/nao/naoqi/Media"
    else
        chown nao:nao /home/nao/naoqi/Media;
        chmod 744 /home/nao/naoqi/Media;
    fi
fi

# add link to the Config directory
if [ ! -h /home/nao/Media ]
then
    echo "setting link to NaoTH Media directory";
    ln -s /home/nao/naoqi/Media /home/nao/Media;
fi

# ----------- stop naoqi -----------

/etc/init.d/naoqi stop

# ----------- copy naoth binaries -----------

rm -f /home/nao/bin/naoth.bak
copy ../home/nao/bin/naoth /home/nao/bin/naoth nao 755
rm -f /home/nao/bin/libnaosmal.so.bak
copy ../home/nao/bin/libnaosmal.so /home/nao/bin/libnaosmal.so nao 755

if [ -d "/home/nao/naoqi/Config" ]; then
  rm -rf /home/nao/naoqi/Config/*
fi

cp -r ../home/nao/naoqi/Config/* /home/nao/Config
chown -R nao:nao /home/nao/naoqi/Config;

# remove any old media files
if [ -d "/home/nao/naoqi/Media" ]; then
  rm -rf /home/nao/naoqi/Media/*
fi

# copy the new media files
cp -r ../home/nao/naoqi/Media/* /home/nao/Media
chown -R nao:nao /home/nao/naoqi/Media;

# ----------- copy libs -----------
if [ -d "/home/nao/lib" ]; then
  rm -rf /home/nao/lib/*
fi

cp -r ../home/nao/lib/* /home/nao/lib

# -----------  system -----------

# Check and Update Runlevel Configuration for Non-Network Services
chown root:root ./checkRC.sh;
chmod 744 ./checkRC.sh;
./checkRC.sh "naoth=default netmount=disable naopathe=disable vsftpd=disable ofono=disable nginx=disable proxydaemon=disable savecache=disable savecache=boot checkpart=disable checkpart-dummy=boot";

# disable the hal-prestarter that takes quite a long time on boot
if [ -f /usr/libexec/hal-prestarter ] ; then
  mv -n /usr/libexec/hal-prestarter /usr/libexec/hal-prestarter.orig
fi

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

# WLAN Encryption
copy ./etc/wpa_supplicant/wpa_supplicant.conf /etc/wpa_supplicant/wpa_supplicant.conf root 644

# Network IP Adresses
copy ./etc/conf.d/net /etc/conf.d/net root 644

# hostname
copy ./etc/conf.d/hostname /etc/conf.d/hostname root 644
copy ./etc/hostname /etc/hostname root 644

# Check and Update Runlevel Configuration for Network Services
chown root:root ./checkRC.sh;
chmod 744 ./checkRC.sh;
./checkRC.sh "connman=disable net.eth0=boot net.wlan0=boot savecache=boot checkpart=disable checkpart-dummy=boot";

# ----------------------

# configure the lib dependencies
ldconfig;

# allow everyone to shutdown
chmod +s /sbin/shutdown
chmod +s /sbin/reboot

su nao -c "/usr/bin/paplay /home/nao/naoqi/Media/usb_stop.wav"

# prevent reboot if appropiate file exists
if [ ! -f "./noreboot" ]; then
	reboot
fi

echo "DONE"

