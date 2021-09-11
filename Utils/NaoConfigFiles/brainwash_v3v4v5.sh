#!/bin/bash

HEAD_ID=$(grep "RobotConfig/Head/FullHeadId" /var/persistent/media/internal/DeviceHeadInternalGeode.xml | sed 's/.*value="\([^"]\+\)".*/\1/')
N=$(grep "$HEAD_ID" robots.cfg | sed 's/.*=\([0-9]\+\)/\1/')
if [[ -z "$N" ]]; then N=99; fi # fallback number 99 !

# ==================== fun definition ====================

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

# ==================== pre stuff ====================

cd deploy/v3v4v5

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

# stop naoqi and naoth
naoth stop
/etc/init.d/naoqi stop

# ==================== boot stuff ====================

# make boot faster
copy ./etc/init.d/checkpart-dummy /etc/init.d/checkpart-dummy root 755

# disable the hal-prestarter that takes quite a long time on boot
if [ -f /usr/libexec/hal-prestarter ] ; then
  mv -n /usr/libexec/hal-prestarter /usr/libexec/hal-prestarter.orig
fi

# ==================== system stuff ====================

# NaoTH init script
copy ./etc/init.d/naoth /etc/init.d/naoth root 755

# Needed by NaoTH init script
copy ./etc/init.d/cognition-common /etc/init.d/cognition-common root 755

# brainwashinit
copy ./usr/bin/brainwash /usr/bin/brainwash root 755

# NaoTH binary start script
copy ./usr/bin/naoth /usr/bin/naoth root 755

# brainwash udev rule
copy ./etc/udev/rules.d/brainwashing.rules /etc/udev/rules.d/brainwashing.rules nao 644
copy ./etc/udev/rules.d/disable-wifi-power-save.rules /etc/udev/rules.d/disable-wifi-power-save.rules nao 644

# copy ld.so.conf
copy ./etc/ld.so.conf /etc/ld.so.conf root 644

# copy new fstab
copy ./etc/fstab /etc/fstab root 644

# add syslogger config
copy ./etc/syslog.conf /etc/syslog.conf root 644

#SSH Config
copy ./etc/ssh.conf/sshd_config /etc/ssh.conf/sshd_config root 644

#RC Config
copy ./etc/rc.conf /etc/rc.conf root 644

# ==================== network stuff ====================

# NOTE: defined as environmental variables in the main script
#NETWORK_WLAN_SSID="NAONET"
#NETWORK_WLAN_PW="a1b0a1b0a1"
#NETWORK_WLAN_IP="10.0.4"
#NETWORK_WLAN_MASK="255.255.255.0"
#NETWORK_WLAN_BROADCAST="10.0.4.255"

#NETWORK_ETH_IP="192.168.13"
#NETWORK_ETH_MASK="255.255.255.0"
#NETWORK_ETH_BROADCAST="192.168.13.255"

NETWORK_WLAN_IP="$NETWORK_WLAN_IP.$N"
NETWORK_ETH_IP="$NETWORK_ETH_IP.$N"

# generate linux network configuration
cat << EOF > /etc/conf.d/net
config_wlan0="$NETWORK_WLAN_IP netmask $NETWORK_WLAN_MASK broadcast $NETWORK_WLAN_BROADCAST"
config_eth0="$NETWORK_ETH_IP netmask $NETWORK_ETH_MASK broadcast $NETWORK_ETH_BROADCAST"
wpa_supplicant_wlan0="-Dnl80211"
EOF

# generate wpa_supplicant configuration
cat << EOF > /etc/wpa_supplicant/wpa_supplicant.conf
ctrl_interface=/var/run/wpa_supplicant
ctrl_interface_group=0
ap_scan=1

network={
  ssid="$NETWORK_WLAN_SSID"
  key_mgmt=WPA-PSK
  psk="$NETWORK_WLAN_PW"
  priority=5
}
EOF

# hostname
echo "hostname=\"nao$N\"" > /etc/conf.d/hostname
echo "nao$N" > /etc/hostname

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

# ==================== libs stuff ====================

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

# ==================== copy stuff ====================

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

# naoqi user autoload.ini
copy ./home/nao/naoqi/preferences/autoload.ini /home/nao/naoqi/preferences/autoload.ini nao 644

# naoqi system autoload.ini
copy ./etc/naoqi/autoload.ini /etc/naoqi/autoload.ini root 644


# -----------  video driver -----------

# copy the video driver
kernel_name=$(uname -r)
video_driver_path=/lib/modules/$kernel_name/kernel/drivers/media/video/mt9m114.ko
copy ./kernel/drivers/media/video/mt9m114.ko $video_driver_path root 644

# -----------  system -----------

# Check and Update Runlevel Configuration for Non-Network Services
chown root:root ./checkRC.sh;
chmod 744 ./checkRC.sh;
./checkRC.sh "naoth=default netmount=disable naopathe=disable vsftpd=disable ofono=disable nginx=disable proxydaemon=disable savecache=disable savecache=boot checkpart=disable checkpart-dummy=boot";

# Check and Update Runlevel Configuration for Network Services
chown root:root ./checkRC.sh;
chmod 744 ./checkRC.sh;
./checkRC.sh "connman=disable net.eth0=boot net.wlan0=boot savecache=boot checkpart=disable checkpart-dummy=boot";

# ----------------------

# configure the lib dependencies
ldconfig;

# ==================== post stuff ====================

# allow everyone to shutdown
chmod +s /sbin/shutdown
chmod +s /sbin/reboot

su nao -c "/usr/bin/paplay /home/nao/naoqi/Media/usb_stop.wav"

# prevent reboot if appropiate file exists
if [ ! -f "./noreboot" ]; then
	reboot
fi

echo "DONE"
