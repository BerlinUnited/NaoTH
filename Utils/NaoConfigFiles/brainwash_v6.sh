#!/bin/bash

echo "using $BACKUP_DIRECTORY and $DEPLOY_DIRECTORY"

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

	# ==================== boot stuff ====================

	# # make boot faster
	# deployFile "/etc/init.d/checkpart-dummy" "root" "755"

	# # disable the hal-prestarter that takes quite a long time on boot
	# if [ -f /usr/libexec/hal-prestarter ] ; then
	#   mv -n /usr/libexec/hal-prestarter /usr/libexec/hal-prestarter.orig
	# fi

	# ==================== system stuff ====================

	# # NaoTH init script
	# deployFile "/etc/init.d/naoth" "root" "755"

	# # Needed by NaoTH init script
	# deployFile "/etc/init.d/cognition-common" "root" "755"

	# brainwashinit
	deployFile "/usr/bin/brainwash" "root" "755" "v6"

	# NaoTH binary start script
	deployFile "/usr/bin/naoth" "root" "755" "v6"

	# NaoTH systemd service
	deployFile "/etc/systemd/system/naoth.service" "root" "644" "v6"
	deployFile "/etc/conf.d/naoth" "root" "644" "v6"

	if [ ! -d /home/nao/.config/systemd ];	then
		mkdir /home/nao/.config/systemd;
	fi

	if [ ! -d /etc/systemd/system/default.target.wants ];	then
		mkdir /etc/systemd/system/default.target.wants;
	fi
	# add link to enable starting at boot
	if [ ! -h /etc/systemd/system/default.target.wants/naoth.service ];	then
		echo "setting link to naoth.service";
		ln -s /etc/systemd/system/naoth.service /etc/systemd/system/default.target.wants/naoth.service;
	fi

	# disable naoqi completely
 	if [ -h /nao/etc/systemd/user/aldebaran.target.wants/naoqi.service ]; then
		rm /nao/etc/systemd/user/aldebaran.target.wants/naoqi.service
	fi

 	if [ -h /nao/etc/systemd/user/aldebaran.target.wants/naoqi-legacy.service ]; then
		rm /nao/etc/systemd/user/aldebaran.target.wants/naoqi-legacy.service
	fi
	# overwrite lola.service with our version (we dont want naoqi as dependency)
	deployFile "/nao/etc/systemd/user/lola.service" "root" "644" "v6"

	# # add link to enable starting at boot
	# if [ ! -f /etc/systemd/system/multi-user.target.wants/naoth.service ];	then
	# 	echo "setting link to naoth.service";
	# 	ln -s /etc/systemd/system/naoth.service /etc/systemd/system/multi-user.target.wants/naoth.service;
	# fi

	systemctl daemon-reload
	#systemctl enable naoth
	# su -c `systemctl --user daemon-reload` nao
	# su -c `systemctl --user enable naoth` nao

	# brainwash udev rule
	deployFile "/etc/udev/rules.d/brainwashing.rules" "nao" "644" "v6"

	# # copy new fstab
	# deployFile "/etc/fstab" "root" "644"

	# # add syslogger config
	# deployFile "/etc/syslog.conf" "root" "644"

	# #SSH Config
	# deployFile "/etc/ssh.conf/sshd_config" "root" "644"

	# ====================  host stuff ====================

	# hostname
	deployFile "/etc/conf.d/hostname" "root" "644" "v6"
	deployFile "/etc/hostname" "root" "644" "v6"

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

# stop naoqi and naoth
naoth stop
# naoqi stop

# disable /etc overlay
umount -l /etc
setEtc
systemctl restart etc.mount
# TODO: check if this is really needed (overlay should merge base and overlayed directory contents)
setEtc

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


# add link to the Config directory
if [ ! -d /home/root ]
then
    echo "symlinking root directory to /home/root";
		mv /root /home/root
    ln -s /home/root /root;
fi

NAO_NUMBER=$(cat /etc/hostname | grep nao | sed -e 's/nao//g')

deployFile "/home/nao/robocup.conf" "nao" "644" "v6"

# deploy binary
deployFile "/home/nao/bin/naoth" "nao" "755" 

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

# allow everyone to shutdown
chmod +s /sbin/shutdown
chmod +s /sbin/reboot

#remount root in read only mode
mount -o remount,ro /

# ==================== network stuff ====================

WIFI_STATE=$( ifconfig | grep -o wlan0)
if [ -z $WIFI_STATE ]; then
	connmanctl enable wifi
	sleep 0.1
fi
connmanctl scan wifi
CONNMAN_SERVICES=$(connmanctl services)

ETH0_MAC=$(cat /sys/class/net/eth0/address | sed -e 's/://g')
WLAN0_MAC=$(cat /sys/class/net/wlan0/address | sed -e 's/://g')

echo "Setting up wifi configuration for wlan0 (${WLAN0_MAC})"
sed -i -e "s/__NAO__/${NAO_NUMBER}/g" $DEPLOY_DIRECTORY/v6/var/lib/connman/wifi.config
deployFile "/var/lib/connman/wifi.config" "root" "644" "v6"

WIFI_NETWORKS=$(cat /var/lib/connman/wifi.config | grep "Name =" | sed -e "s/ //g" | sed -e "s/Name=//g")
for wifi in $WIFI_NETWORKS; do
	if [ ! $wifi == "wifi" ]; then
		# echo "$CONNMAN_SERVICES" | grep "SPL_A " | grep -o wifi.*
		service=$(echo "$CONNMAN_SERVICES" | grep "$wifi " | grep -o wifi.*)
		if [ ! -z $service ]; then
			echo "Disabling autoconnect on wifi network $wifi"
			# echo $service
			connmanctl config ${service} --autoconnect off
			# echo "connmanctl config ${service} --autoconnect off"
		else
				echo "Wifi network $wifi currently not available"
		fi
	fi
done

# play initial sound
su nao -c "/usr/bin/paplay /home/nao/naoqi/Media/usb_stop.wav"

echo "Setting ip of eth0 (${ETH0_MAC})"
connmanctl config ethernet_${ETH0_MAC}_cable --ipv4 manual 192.168.13.${NAO_NUMBER} 255.255.255.0

# prevent reboot if appropiate file exists
if [ ! -f "./noreboot" ]; then
	reboot
fi


echo "DONE"
