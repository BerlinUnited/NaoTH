#!/bin/bash
# Script must be work with root-permission therefore start as root
# Hint: sudo -E preserves ENVVars
function umount_tmp()
{
    MOUNT_POINT=$1
    echo "Umounting $MOUNT_POINT/home"
    /bin/umount $MOUNT_POINT/home
    echo "Umounting $MOUNT_POINT"
    /bin/umount $MOUNT_POINT

    echo "Deleting $MOUNT_POINT"
    rmdir $MOUNT_POINT
}

# Config-Vars
DEFAULT_DEVICE="/dev/sdb"

#DEFAULT_IMAGE="$NAO_IMAGE_DIR/opennao-robocup-1.6.0-nao-geode.ext3"
#DEFAULT_IMAGE="$NAO_IMAGE_DIR/opennao-robocup-1.8.16-nao-geode.ext3"
#DEFAULT_IMAGE="$NAO_IMAGE_DIR/nao-system-image-robocup-1.10.10-nao-geode-20110111152911.rootfs.ext3"
DEFAULT_IMAGE="$NAO_IMAGE_DIR/nao-system-image-robocup-1.10.37-nao-geode-20110418103858.rootfs.ext3"
DEFAULT_IMAGE_BURN="n"

#ethernet
DEFAULT_ETH_NET="192.168.0."

DEFAULT_WLAN_NET="192.168.13."
DEFAULT_WLAN_SSID="NAONET"
DEFAULT_WLAN_PASSWORD="a1b0a1b0a1"

ALLRC="rcS.d rc0.d rc1.d rc2.d rc3.d rc4.d rc5.d rc6.d"
RUNRC="rc5.d"

# Getting Information and checking

if [ x$(whoami) != xroot ]; then
  echo "You must be root! Will exit"
  exit 1
fi

echo "Please enter path to Device. Press enter for default
[default: $DEFAULT_DEVICE]"
read DEVICE
if [ "$DEVICE" == "" ]
then 
  DEVICE=$DEFAULT_DEVICE
fi

echo "Do you want to burn naoqi-image on the stick?(y/n)
[default: $DEFAULT_IMAGE_BURN]"
read BURN
if [ "$BURN" == "" ]
then
	BURN=$DEFAULT_IMAGE_BURN
fi

echo "Unmounting stick partitions if mounted"
umount ${DEVICE}1
umount ${DEVICE}2

if [ "$BURN" == "Y" ] || [ "$BURN" == "y" ]
then
	echo "Please enter path to Filesystem-Image. Press enter for default"
	echo "[default: $DEFAULT_IMAGE]"
	read IMAGE
	if [ "$IMAGE" == "" ]
	then
  	IMAGE=$DEFAULT_IMAGE
	fi

	if mount | grep $DEVICE >/dev/null 2>/dev/null
	then
  	echo "Error: Please umount: $DEVICE"
  	exit 3
	fi
	
	# Process

	echo "Creating new system on $DEVICE what's the stick's size(1, 2 or 4)GB [default: 2]?"
	read DEVSIZE
	if [ "$DEVSIZE" == "" ]
	then
	    DEVSIZE=2
	fi
	if [ ${DEVSIZE} == 1 ] || [ ${DEVSIZE} == 2 ] || [ ${DEVSIZE} == 4 ]; then
	    if [ -f $AL_DIR/share/flash-usbstick/first_sector_${DEVSIZE}GB.raw ];then
		echo "write MBR for usbstick ${DEVSIZE}G"
		dd if="${AL_DIR}/share/flash-usbstick/first_sector_${DEVSIZE}GB.raw" of=$DEVICE bs=16k || exit 1
	    else
		echo "You need file : ${AL_DIR}/share/flash-usbstick/first_sector_${DEVSIZE}GB.raw" && exit 1
	    fi

#	    parted $DEVICE --script mklabel msdos
#	    parted $DEVICE --script mkpart primary 0 512M
#	    mkfs.ext3 -q "${DEVICE}1"
#	    parted $DEVICE --script mkpart primary 512M
#	    sleep 2
#	    mkfs.ext3 -q "${DEVICE}1"
	    echo "writing image to stick"
	    #gzip -dc $IMAGE | dd of=$DEVICE bs=16k seek=1 conv=notrunc	|| 
	    dd if=$IMAGE of=$DEVICE bs=16384 seek=1 conv=notrunc || exit 1
	    echo "writing complete"
	    
#	    parted $DEVICE --script mklabel msdos
#	    parted $DEVICE --script rescue 0 512M
#	    parted $DEVICE --script set 1 boot on
#	    parted $DEVICE --script mkpartfs primary ext2 512 -1
#	    sleep 2
#	    mkfs.ext3 -q "${DEVICE}2"

	fi
#	sfdisk -R $DEVICE 2>/dev/null
	sleep 5
fi

#mounting 

TMP_MOUNT_POINT=$(mktemp -d)
echo "Mounting Filesystem $TMP_MOUNT_POINT"
mount ${DEVICE}1 -t ext3 $TMP_MOUNT_POINT

if [ "$BURN" == "N" ] || [ "$BURN" == "n" ]
then
    echo "Mounting Filesystem $TMP_MOUNT_POINT/home"
    mount ${DEVICE}2 -t ext3 $TMP_MOUNT_POINT/home
fi

if [ $? -ne 0 ]
then
  echo "Unable to mount filesystem!"
  exit 1
fi

# change current working dir to stick

#cd $TMP_MOUNT_POINT

echo $TMP_MOUNT_POINT

if [ "$BURN" == "Y" ] || [ "$BURN" == "y" ]
then
#naonr

while [ TRUE ]
do
  echo "Please enter Nao-Nr:"
  read NAONR
  if [ $NAONR != "" ] && [ $NAONR -lt 255 ] && [ $NAONR -gt 0 ]
  then
    break
  else 
    echo "Error: Please enter Number between 1 and 254"
  fi
done

#wifi

cp $NAOTH_BZR/Misc/NaoConfigFiles/etc/wpa_supplicant.conf $TMP_MOUNT_POINT/etc/wpa_supplicant.conf

echo "Please enter WLAN-SSID for Nao. Press enter for default
[default: $DEFAULT_WLAN_SSID]"
read WLAN_SSID
if [ "$WLAN_SSID" == "" ]
then
	WLAN_SSID=$DEFAULT_WLAN_SSID
fi

echo "Setting WLAN-SSID"
cp $TMP_MOUNT_POINT/etc/wpa_supplicant.conf $TMP_MOUNT_POINT/etc/wpa_supplicant.conf.backup
sed 's/'ssid=.\*'/'ssid=\"${WLAN_SSID}\"'/' $TMP_MOUNT_POINT/etc/wpa_supplicant.conf.backup > $TMP_MOUNT_POINT/etc/wpa_supplicant.conf
rm $TMP_MOUNT_POINT/etc/wpa_supplicant.conf.backup


echo "Please enter WLAN-PASSWORD for Nao. Press enter for default
[default: $DEFAULT_WLAN_PASSWORD]"
read WLAN_PASSWORD
if [ "$WLAN_PASSWORD" == "" ]
then
	WLAN_PASSWORD=$DEFAULT_WLAN_PASSWORD
fi

echo "Setting WLAN-PASSWORD"
cp $TMP_MOUNT_POINT/etc/wpa_supplicant.conf $TMP_MOUNT_POINT/etc/wpa_supplicant.conf.backup
sed 's/'wep_key0=.\*'/'wep_key0=${WLAN_PASSWORD}'/' $TMP_MOUNT_POINT/etc/wpa_supplicant.conf.backup > $TMP_MOUNT_POINT/etc/wpa_supplicant.conf
rm $TMP_MOUNT_POINT/etc/wpa_supplicant.conf.backup

# add a service that starts wpa_supplicant on every start
echo "adding /etc/init.d/wpa_supplicant"
cp -f $NAOTH_BZR/Misc/NaoConfigFiles/wpa_supplicant $TMP_MOUNT_POINT/etc/init.d/wpa_supplicant
chmod ugo+x $TMP_MOUNT_POINT/etc/init.d/wpa_supplicant
for R in $RUNRC; do
    ln -sf ../init.d/wpa_supplicant $TMP_MOUNT_POINT/etc/$R/S99wpa_supplicant
done

#ethernet

echo "Please enter Ethernet-Address for Nao. Press enter for default
[default: $DEFAULT_ETH_NET$NAONR]"
read ETH_ADDR
if [ "$ETH_ADDR" == "" ]
then 
  ETH_ADDR=$DEFAULT_ETH_NET$NAONR
  ETH_PREFIX=$DEFAULT_ETH_NET
else
  ETH_PREFIX= echo $ETH_ADDR | egrep -o '^[0-9]+\.[0-9]+\.[0-9]+\.'
fi

echo "Please enter WLAN-Address for Nao. Press enter for default
[default: $DEFAULT_WLAN_NET$NAONR]"
read WLAN_ADDR
if [ "$WLAN_ADDR" == "" ]
then 
  WLAN_ADDR=$DEFAULT_WLAN_NET$NAONR
  WLAN_PREFIX=$DEFAULT_WLAN_NET
else
  WLAN_PREFIX= echo $ETH_ADDR | egrep -o '^[0-9]+\.[0-9]+\.[0-9]+\.'
fi

echo "Setting Ethernet-Address on $ETH_ADDR and WLAN-Address on $WLAN_ADDR"
sed -e 's/'ETH_ADDR'/'${ETH_ADDR}'/' -e 's/'ETH_NET'/'${ETH_PREFIX}0'/' -e 's/'ETH_GATE'/'${ETH_PREFIX}1'/' -e 's/'WLAN_ADDR'/'${WLAN_ADDR}'/' -e 's/'WLAN_NET'/'${WLAN_PREFIX}0'/' -e 's/'WLAN_GATE'/'${WLAN_PREFIX}1'/'   $NAOTH_BZR/Misc/NaoConfigFiles/etc/network/interfaces > $TMP_MOUNT_POINT/etc/network/interfaces

echo "Copying staging area"
cp -R $NAO_CROSSCOMPILE/staging/usr/ $TMP_MOUNT_POINT/

    echo "put the stick into the nao, boot once and then run this script again without burning the image onto it"
else # if burn
###################################
echo "Query for needed user and group IDs..."
NAO_UID=`cat $TMP_MOUNT_POINT/etc/passwd | grep '^nao:' | cut -f "3" -d ':'`
NAO_GID=`cat $TMP_MOUNT_POINT/etc/passwd | grep '^nao:' | cut -f "4" -d ':'`

echo "nao user: UID=$NAO_UID GID=$NAO_GID"

if [ "$NAO_UID" == "" ]
then
  echo "Could not find the UID for 'nao'"
  umount_tmp "$TMP_MOUNT_POINT"
  exit 1
fi
if [ "$NAO_GID" == "" ]
then
  echo "Could not find the GID for 'nao'"
  umount_tmp "$TMP_MOUNT_POINT"
  exit 1
fi

####################################
echo "Removing needless services..."

FILES="openntpd avahi-daemon lighttpd connman vsftpd udhcpc logread boot-progress checkpart rmnologin naopathe nao.fcgi"
#getty 

for R in $ALLRC; do
  for F in $FILES; do
    rm -f $TMP_MOUNT_POINT/etc/$R/*$F*
  done
done  

####################################

echo "adding /etc/init.d/cognition-common"
cp -f $NAOTH_BZR/Misc/NaoConfigFiles/cognition-common $TMP_MOUNT_POINT/etc/init.d/cognition-common
chmod ugo+x $TMP_MOUNT_POINT/etc/init.d/cognition-common

echo "adding /etc/init.d/naoth"
cp -f $NAOTH_BZR/Misc/NaoConfigFiles/naoth $TMP_MOUNT_POINT/etc/init.d/naoth
chmod ugo+x $TMP_MOUNT_POINT/etc/init.d/naoth
for R in $RUNRC; do
    ln -sf ../init.d/naoth $TMP_MOUNT_POINT/etc/$R/S99naoth
done

####################################
echo "Creating directories..."

mkdir -p $TMP_MOUNT_POINT/home/nao/naoqi/Config/
mkdir -p $TMP_MOUNT_POINT/home/nao/naoqi/lib/naoqi/
mkdir -p $TMP_MOUNT_POINT/home/nao/naoqi/preferences/

####################################
echo "Copying autoload.ini..."
cp $NAOTH_BZR/Misc/NaoConfigFiles/autoload.ini $TMP_MOUNT_POINT/home/nao/naoqi/preferences/

####################################

# naoth-configuration
echo "" > $TMP_MOUNT_POINT/home/FORMATED
cp -rf $NAOTH_BZR/NaoTHSoccer/Config $TMP_MOUNT_POINT/home/nao/naoqi/
ln -s /home/nao/naoqi/Config/ $TMP_MOUNT_POINT/opt/naoqi/Config

while [ TRUE ]
do
	echo "Please enter Team-number:"
	read TEAMNR
	if [ $TEAMNR != "" ] && [ $TEAMNR -lt 250 ] && [ $TEAMNR -gt 0 ]
	then
		break
	else
		echo "Error: Please enter Number between 0 and 250"
	fi
done

(echo "[player]" > $TMP_MOUNT_POINT/home/nao/naoqi/Config/private/player.cfg)
(echo "TeamNumber=$TEAMNR" >> $TMP_MOUNT_POINT/home/nao/naoqi/Config/private/player.cfg)

while [ TRUE ]
do
	echo "Please enter Team-color(red=0,blue=1):"
	read TEAMCOLOR
	if [ $TEAMCOLOR != "" ] && [ $TEAMCOLOR -le 1 ] && [ $TEAMCOLOR -ge 0 ]
	then
		break
	else
		echo "Error: Please choose between 0 and 1"
	fi
done

if [ $TEAMCOLOR -eq 0 ]
then
	(echo "TeamColor=red" >> $TMP_MOUNT_POINT/home/nao/naoqi/Config/private/player.cfg)
else
	(echo "TeamColor=blue" >> $TMP_MOUNT_POINT/home/nao/naoqi/Config/private/player.cfg)
fi

while [ TRUE ]
do
	echo "Please enter Player-number:"
	read PLAYERNR
	if [ $PLAYERNR != "" ] && [ $PLAYERNR -le 5 ] && [ $PLAYERNR -ge 0 ]
	then
		break
	else
		echo "Error: Please enter Number between 0 and 5"
	fi
done

(echo "PlayerNumber=$PLAYERNR" >> $TMP_MOUNT_POINT/home/nao/naoqi/Config/private/player.cfg)

echo "copying libnaoth.so"
cp -f ${NAOTH_BZR}/NaoTHSoccer/dist/Nao/libnaoth.so $TMP_MOUNT_POINT/home/nao/naoqi/lib/naoqi/
echo "copying naoth executable"
mkdir -p $TMP_MOUNT_POINT/home/nao/bin/
cp -f ${NAOTH_BZR}/NaoTHSoccer/dist/Nao/naoth $TMP_MOUNT_POINT/home/nao/bin/


echo "Changing file permissions..."
chown -R $NAO_UID:$NAO_GID $TMP_MOUNT_POINT/home/nao/naoqi/
chown -R $NAO_UID:$NAO_GID $TMP_MOUNT_POINT/home/nao/bin/

fi


# cleaning

cd ..
umount_tmp "$TMP_MOUNT_POINT"

echo "Finished!"

