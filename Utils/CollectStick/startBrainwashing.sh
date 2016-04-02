#!/bin/bash

#INFO: file is executed via root on the nao!

# set file vars
infoFile="/home/nao/Config/nao.info"
errorFile="/home/nao/brainwasher.log"

# set vars
current_date=$(date +"%y%m%d-%H%M")
current_nao=$(sed -n "2p" $infoFile)
current_boot_time=$(</proc/uptime awk '{printf "%d", $1 / 60}')

check_for_errors() {
	if ["$?" -ne 0]
	then
		sudo -u nao /usr/bin/paplay /home/nao/naoqi/Media/1.wav
	fi
}

# write to systemlog
logger "Brainwasher:start $current_date, $current_nao"

# play sound
sudo -u nao /usr/bin/paplay /home/nao/naoqi/Media/usb_stop.wav

naoth stop

# write previous volume to systemlog
current_volume=$(sudo -u nao pactl list sinks | grep Volume | xargs)
logger "Brainwasher:$current_volume"

# set volume to 88%
sudo -u nao pactl set-sink-mute 0 false 2> $errorFile
sudo -u nao pactl set-sink-volume 0 88% 2>> $errorFile
logger -f $errorFile

logger "Brainwasher:copy files"
# create directory
mkdir $current_date-$current_nao

# find log files and copy them to the created directory
#find -L /tmp -type d -name media -prune -o -name "*.log" -exec cp {} /media/brainwasher/$current_date-$current_nao \;
# find log files, create MD5 hashes and copy everything to the created directory
for f in $(find -L /tmp -type d -name media -prune -o -name "*.log")
do
	md5sum $f | sed -e "s/\/tmp\///g" > "$f.md5"
	cp "$f.md5" $f /media/brainwasher/$current_date-$current_nao/
	check_for_errors
done

# copy the config directory
cp -r /home/nao/naoqi/Config /media/brainwasher/$current_date-$current_nao
check_for_errors

# create dump folder and log errors
mkdir -p /media/brainwasher/$current_date-$current_nao/dumps 2> $errorFile
logger -f $errorFile

# find and copy trace dump files since boot and log errors
find /home/nao -maxdepth 1 -type f -mmin -$current_boot_time -iname "trace.dump.*" -exec cp {} /media/brainwasher/$current_date-$current_nao/dumps/ \; 2> $errorFile
logger -f $errorFile

# remove error log file
rm $errorFile

# make sure, everything is written
sync

logger "Brainwasher:start naoth"

naoth restart

logger "Brainwasher:end"

sudo -u nao /usr/bin/paplay /home/nao/naoqi/Media/nicknacknuck.wav