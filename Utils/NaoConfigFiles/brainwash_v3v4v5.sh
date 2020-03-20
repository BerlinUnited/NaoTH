#!/bin/bash


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

# ==================== pre stuff ====================

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
/etc/init.d/naoqi stop

# ==================== boot stuff ====================

# make boot faster
deployFile "/etc/init.d/checkpart-dummy" "root" "755" "v3v4v5"

# disable the hal-prestarter that takes quite a long time on boot
if [ -f /usr/libexec/hal-prestarter ] ; then
  mv -n /usr/libexec/hal-prestarter /usr/libexec/hal-prestarter.orig
fi

# ==================== system stuff ====================

# NaoTH init script
deployFile "/etc/init.d/naoth" "root" "755" "v3v4v5"

# Needed by NaoTH init script
deployFile "/etc/init.d/cognition-common" "root" "755" "v3v4v5"

# brainwashinit
deployFile "/usr/bin/brainwash" "root" "755" "v3v4v5"

# NaoTH binary start script
deployFile "/usr/bin/naoth" "root" "755" "v3v4v5"

# brainwash udev rule
deployFile "/etc/udev/rules.d/brainwashing.rules" "nao" "644" "v3v4v5"
deployFile "/etc/udev/rules.d/disable-wifi-power-save.rules" "nao" "644" "v3v4v5"

# copy ld.so.conf
deployFile "/etc/ld.so.conf" "root" "644" "v3v4v5"

# copy new fstab
deployFile "/etc/fstab" "root" "644" "v3v4v5"

# add syslogger config
deployFile "/etc/syslog.conf" "root" "644" "v3v4v5"

#SSH Config
deployFile "/etc/ssh.conf/sshd_config" "root" "644" "v3v4v5"

#RC Config
deployFile "/etc/rc.conf" "root" "644" "v3v4v5"

# ==================== network stuff ====================

# WLAN Encryption
deployFile "/etc/wpa_supplicant/wpa_supplicant.conf" "root" "644" "v3v4v5"

# Network IP Adresses
deployFile "/etc/conf.d/net" "root" "644" "v3v4v5"

# hostname
deployFile "/etc/conf.d/hostname" "root" "644" "v3v4v5"
deployFile "/etc/hostname" "root" "644" "v3v4v5"

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

if [ -d "$DEPLOY_DIRECTORY/home/nao/lib" ]; then
	logger -t naoth "Deploy libs"
	# update the system wide libstdc++
	if [ -f "$DEPLOY_DIRECTORY/home/nao/lib/libstdc++.so" ]
	then
		mkdir -p $BACKUP_DIRECTORY/usr/lib/
	    mv -f /usr/lib/libstdc++.* $BACKUP_DIRECTORY/usr/lib/
	    cp $DEPLOY_DIRECTORY/home/nao/lib/libstdc++.* /usr/lib/
	    # copy user and access rights
	    chown root:root /usr/lib/libstdc++.*
	    chmod 755 /usr/lib/libstdc++.*
	fi

	# copy local libs
	deployDirectory "/home/nao/lib" "nao" "744"

	# configure the lib dependencies
	ldconfig
fi

# ==================== copy stuff ====================

# deploy binaries
deployFile "/home/nao/bin/libnaosmal.so" "nao" "755"
deployFile "/home/nao/bin/naoth" "nao" "755"

# deploy media
deployDirectory "/home/nao/naoqi/Media" "nao" "744"

# add link to the Media directory
if [ ! -h /home/nao/Media ]
then
    echo "setting link to NaoTH Media directory";
    ln -s /home/nao/naoqi/Media /home/nao/Media;
fi

# naoqi user autoload.ini
deployFile "/home/nao/naoqi/preferences/autoload.ini" "nao" "644" "v3v4v5"

# naoqi system autoload.ini
deployFile "/etc/naoqi/autoload.ini" "root" "644" "v3v4v5"

# create the local Config directory
deployDirectory "/home/nao/naoqi/Config" "nao" "744"

# add link to the Config directory
if [ ! -h /home/nao/Config ]
then
    echo "setting link to NaoTH Config directory";
    ln -s /home/nao/naoqi/Config /home/nao/Config;
fi

# -----------  video driver -----------

# copy the video driver
#kernel_name=$(uname -r)
#video_driver_path=/lib/modules/$kernel_name/kernel/drivers/media/video/mt9m114.ko
#copy ./kernel/drivers/media/video/mt9m114.ko $video_driver_path root 644

# Check and Update Runlevel Configuration for Non-Network Services
#chown root:root ./checkRC.sh;
#chmod 744 ./checkRC.sh;
#./checkRC.sh "naoth=default netmount=disable naopathe=disable vsftpd=disable ofono=disable nginx=disable proxydaemon=disable savecache=disable savecache=boot checkpart=disable checkpart-dummy=boot";

# Check and Update Runlevel Configuration for Network Services
#chown root:root ./checkRC.sh;
#chmod 744 ./checkRC.sh;
#./checkRC.sh "connman=disable net.eth0=boot net.wlan0=boot savecache=boot checkpart=disable checkpart-dummy=boot";

# ==================== post stuff ====================

# allow everyone to shutdown
chmod +s /sbin/shutdown
chmod +s /sbin/reboot

# prevent reboot if appropiate file exists
if [ ! -f "./noreboot" ]; then
	reboot
fi

# play initial sound
su nao -c "/usr/bin/paplay /home/nao/naoqi/Media/usb_stop.wav"

echo "DONE"
