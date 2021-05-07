#!/bin/bash

echo "using $BACKUP_DIRECTORY and $DEPLOY_DIRECTORY"

HEAD_ID=$(cat /sys/qi/head_id)
N=$(grep "$HEAD_ID" robots.cfg | sed 's/.*=\([0-9]\+\)/\1/')
if [[ -z "$N" ]]; then N=99; fi # fallback number 99 !

# ==================== fun definition ====================

deployFile() {
	local FILE="$1"
	local OWNER="$2"
	local RIGHTS="$3"
	local PREFIX="$4"

	if [ -z $PREFIX ]; then
		SOURCEFILE="$DEPLOY_DIRECTORY/$FILE"
	else
		SOURCEFILE="$DEPLOY_DIRECTORY/$PREFIX/$FILE"
	fi

	echo "$DEPLOY_DIRECTORY/$SOURCEFILE -> $FILE"

	if [ -f "$SOURCEFILE" ]; then
		logger -t naoth "Deploy file '$SOURCEFILE' for $OWNER with $RIGHTS"
		# backup existing file
		if [ -f "$FILE" ]; then
			mkdir -p $BACKUP_DIRECTORY/$(dirname $FILE)
			mv -f $FILE $BACKUP_DIRECTORY/$FILE
		fi
		cp -f $SOURCEFILE $FILE
		# copy user and access rights
		chown $OWNER:$OWNER $FILE
		chmod $RIGHTS $FILE
	fi
}


deployDirectory() {
	local DIR="$1"
	local OWNER="$2"
	local RIGHTS="$3"
	local PREFIX="$4"

	if [ -z $PREFIX ]; then
		SOURCEDIR="$DEPLOY_DIRECTORY/$DIR"
	else
		SOURCEDIR="$DEPLOY_DIRECTORY/$PREFIX/$DIR"

	fi
	
	echo "$SOURCEDIR -> $DIR"

	if [ -d "$SOURCEDIR" ]; then
		logger -t naoth "Deploy directory '$SOURCEDIR' for $OWNER with $RIGHTS"
		# backup directory files
		if [ -d "$DIR" ]; then
			mkdir -p $BACKUP_DIRECTORY/$DIR
			mv -f $DIR/* $BACKUP_DIRECTORY/$DIR/
		fi
		rm -rf $DIR

		# copy files
		cp -r $SOURCEDIR $DIR

		chown -R $OWNER:$OWNER $DIR
		chmod -R $RIGHTS $DIR
	fi
}


setEtc(){

	# ==================== system stuff ====================

	# NaoTH systemd service environment file
	deployFile "/etc/conf.d/naoth" "root" "644" "v6"

	# brainwash udev rule
	deployFile "/etc/udev/rules.d/brainwashing.rules" "root" "644" "v6"

	# ====================  host stuff ====================

	# hostname
	echo "nao$N" > /etc/hostname

	# ====================  pulseaudio stuff ====================

	deployFile "/etc/pulse/client-multi-user.conf" "root" "644" "v6"
	deployFile "/etc/pulse/default-multi-user.pa" "root" "644" "v6"

	# NOTE: usage of nested quatation marks is intended. $(...) creates
	# a new context for quotation marks. Explained here:
	# https://unix.stackexchange.com/a/118438
	if [ -z "$(grep "multi-user" /etc/pulse/client.conf)" ]; then
		echo ".include /etc/pulse/client-multi-user.conf" >> /etc/pulse/client.conf
	fi

	if [ -z "$(grep "multi-user" /etc/pulse/default.pa)" ]; then
		echo ".include /etc/pulse/default-multi-user.pa" >> /etc/pulse/default.pa
	fi
}

setNetwork(){
	systemctl stop connman
	systemctl disable connman

	systemctl stop connman-wait-online
	systemctl disable connman-wait-online


	systemctl stop wpa_supplicant
	systemctl disable wpa_supplicant

	systemctl stop avahi-daemon
	systemctl disable avahi-daemon

	deployFile "/lib/systemd/system/net.eth0.service" "root" "644" "v6"
	deployFile "/lib/systemd/system/net.wlan0.service" "root" "644" "v6"

	systemctl enable net.eth0
	systemctl enable net.wlan0
}


# ==================== pre stuff ====================

#remount root in read write mode
mount -o remount,rw /

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
su nao -c "/usr/bin/paplay $DEPLOY_DIRECTORY/home/nao/naoqi/Media/usb_start.wav"

# stop naoth
naoth stop

# brainwashinit
deployFile "/usr/bin/brainwash" "root" "755" "v6"

# NaoTH binary start script
deployFile "/usr/bin/naoth" "root" "755" "v6"
deployFile "/usr/bin/lola_adaptor" "root" "755" "v6"

# ==================== etc stuff ====================

# disable /etc overlay
umount -l /etc
setEtc
setNetwork
systemctl restart etc.mount

# ==================== boot/user service stuff ====================

# disable naoqi completely
if [ -h /nao/etc/systemd/user/aldebaran.target.wants/naoqi.service ]; then
	rm /nao/etc/systemd/user/aldebaran.target.wants/naoqi.service
fi

if [ -h /nao/etc/systemd/user/aldebaran.target.wants/naoqi-legacy.service ]; then
	rm /nao/etc/systemd/user/aldebaran.target.wants/naoqi-legacy.service
fi

# overwrite lola.service with our version (we dont want naoqi as dependency)
deployFile "/nao/etc/systemd/user/lola.service" "root" "644" "v6"
# overwrite pulseaudio.service with our version (we dont want naoqi as dependency)
deployFile "/nao/etc/systemd/user/pulseaudio.service" "root" "644" "v6"

deployFile "/nao/etc/systemd/user/lola_adaptor.service" "root" "644" "v6"
deployFile "/nao/etc/systemd/user/naoth.service" "root" "644" "v6"

if [ ! -h /nao/etc/systemd/user/aldebaran.target.wants/lola_adaptor.service ];	then
	echo "setting link to lola_adaptor.service";
	ln -s /nao/etc/systemd/user/lola_adaptor.service /nao/etc/systemd/user/aldebaran.target.wants/lola_adaptor.service;
fi

if [ ! -h /nao/etc/systemd/user/aldebaran.target.wants/naoth.service ];	then
	echo "setting link to naoth.service";
	ln -s /nao/etc/systemd/user/naoth.service /nao/etc/systemd/user/aldebaran.target.wants/naoth.service;
fi

# ==================== libs stuff ====================

# add lib directory
if [ ! -d /home/nao/lib ]
then
	echo "creating directory /home/nao/lib";
	mkdir /home/nao/lib
	chown nao:nao /home/nao/lib
	chmod 744 /home/nao/lib
fi

# add bin directory
if [ ! -d /home/nao/bin ]
then
	echo "creating directory /home/nao/bin";
	mkdir /home/nao/bin
	chown nao:nao /home/nao/bin
	chmod 744 /home/nao/bin
fi

if [ -f ./deploy/v6/home/nao/lib/libprotobuf.so.14 ]; then
	deployFile "/home/nao/lib/libprotobuf.so.14" "nao" "644"
else
	deployFile "/home/nao/lib/libprotobuf.so" "nao" "644" 
	mv /home/nao/lib/libprotobuf.so /home/nao/lib/libprotobuf.so.14
fi

if [ -f ./deploy/v6/home/nao/lib/libfftw3.so.3 ]; then
	deployFile "/home/nao/lib/libfftw3.so.3" "nao" "644"
else
	deployFile "/home/nao/lib/libfftw3.so" "nao" "644"
	mv /home/nao/lib/libfftw3.so /home/nao/lib/libfftw3.so.3
fi

if [ -f ./deploy/v6/home/nao/lib/libpulse-simple.so.0 ]; then
	deployFile "/home/nao/lib/libpulse-simple.so.0" "nao" "644"
else
	deployFile "/home/nao/lib/libpulse-simple.so" "nao" "644"
	mv /home/nao/lib/libpulse-simple.so /home/nao/lib/libpulse-simple.so.0
fi

if [ -f ./deploy/v6/home/nao/lib/libpulse.so.0 ]; then
	deployFile "/home/nao/lib/libpulse.so.0" "nao" "644"
else
	deployFile "/home/nao/lib/libpulse.so" "nao" "644"
	mv /home/nao/lib/libpulse.so /home/nao/lib/libpulse.so.0
fi

deployFile "/home/nao/lib/libpulsecommon-3.99.so" "nao" "644" 

if [ -f ./deploy/v6/home/nao/lib/libjson.so.0 ]; then
	deployFile "/home/nao/lib/libjson.so.0" "nao" "644"
else
	deployFile "/home/nao/lib/libjson.so" "nao" "644" 
	mv /home/nao/lib/libjson.so /home/nao/lib/libjson.so.0
fi

if [ -f ./deploy/v6/home/nao/lib/libgdbm.so.3 ]; then
	deployFile "/home/nao/lib/libgdbm.so.3" "nao" "644" 
else
	deployFile "/home/nao/lib/libgdbm.so" "nao" "644" 
	mv /home/nao/lib/libgdbm.so /home/nao/lib/libgdbm.so.3
fi

# ==================== copy stuff ====================
deployFile "/home/nao/robocup.conf" "nao" "644" "v6"
deployFile "/home/nao/.profile" "nao" "644"

# deploy binary
deployFile "/home/nao/bin/naoth" "nao" "755" 

# deploy binary
deployFile "/home/nao/bin/lola_adaptor" "nao" "755" 

# deploy media
deployDirectory "/home/nao/naoqi/Media" "nao" "744"

# add link to the Media directory
if [ ! -h /home/nao/Media ]
then
    echo "setting link to NaoTH Media directory";
    ln -s /home/nao/naoqi/Media /home/nao/Media;
fi

# # naoqi user autoload.ini
# deployFile "/home/nao/naoqi/preferences/autoload.ini" "nao" "644"

# # naoqi system autoload.ini
# deployFile "/etc/naoqi/autoload.ini" "root" "644"

# create the local Config directory
deployDirectory "/home/nao/naoqi/Config" "nao" "744"

# add link to the Config directory
if [ ! -h /home/nao/Config ]
then
    echo "setting link to NaoTH Config directory";
    ln -s /home/nao/naoqi/Config /home/nao/Config;
fi

echo "bodyID: ALDTxxx<\nnickName:nao" > /home/nao/Config/nao.info	
chown nao:nao /home/nao/Config/nao.info
chmod 644 /home/nao/Config/nao.info

# ==================== post stuff ====================

chown -R nao:nao /home/nao/backup*

#remount root in read only mode
mount -o remount,ro /

# ==================== network stuff ====================

echo "Generate network configuration";

NETWORK_WLAN_SSID="NAONET"
NETWORK_WLAN_PW="a1b0a1b0a1"
NETWORK_WLAN_IP="10.0.4"
NETWORK_WLAN_MASK="255.255.255.0"
NETWORK_WLAN_BROADCAST="10.0.4.255"

NETWORK_ETH_IP="192.168.13"
NETWORK_ETH_MASK="255.255.255.0"
NETWORK_ETH_BROADCAST="192.168.13.255"

NETWORK_WLAN_IP="$NETWORK_WLAN_IP.$N"
NETWORK_ETH_IP="$NETWORK_ETH_IP.$N"

# generate linux network configuration
cat << EOF > /home/nao/.config/net
config_wlan0="$NETWORK_WLAN_IP netmask $NETWORK_WLAN_MASK broadcast $NETWORK_WLAN_BROADCAST"
config_eth0="$NETWORK_ETH_IP netmask $NETWORK_ETH_MASK broadcast $NETWORK_ETH_BROADCAST"
wpa_supplicant_wlan0="-Dnl80211"
EOF

# generate wpa_supplicant configuration
cat << EOF > /home/nao/.config/wpa_supplicant.conf
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

echo "Restart network services";
systemctl restart net.eth0
systemctl restart net.wlan0

# ==================== Done ====================

# prevent reboot if appropiate file exists
if [ ! -f "./noreboot" ]; then
	reboot
fi


echo "DONE"
