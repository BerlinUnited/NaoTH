#!/bin/bash

#INFO: file is executed via root on the nao!
#		- plays "usb_stop.wav" in the beginning
#		- plays "1.wav" if a copy-error occurred (game.log, Config/)
#		- plays "nicknacknuck.wav" at the end

# set file vars
infoFile="/home/nao/Config/nao.info"
errorFile="/home/nao/brainwasher.log"

# set vars
current_date=$(date +"%y%m%d-%H%M")
current_nao=$(sed -n "2p" $infoFile)
current_nao_name=$(cat /etc/hostname) # get the name, eg. "nao96"
current_nao_number=$(cat /etc/hostname | grep -Eo "[0-9]{2}") # get the number, e.g. "96"
current_nao_player=$(cat /home/nao/Config/robots/$current_nao_name/player.cfg | grep -Eo "PlayerNumber=[0-9]{1,2}")

current_boot_time=$(</proc/uptime awk '{printf "%d", $1 / 60}')

check_for_errors() {
	if [ "$?" -ne 0 ]
	then
		sudo -u nao /usr/bin/paplay /home/nao/naoqi/Media/1.wav
		# if argument is available - write to systemlog
		if [ ! -z "$1" ]
		then
			logger "$1"
		fi
	fi
}

# write to systemlog
logger "Brainwasher:start $current_date, $current_nao, Player $current_nao_player"

# play sound
sudo -u nao /usr/bin/paplay /home/nao/naoqi/Media/usb_start.wav

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

# copy info file of the nao
cp $infoFile /media/brainwasher/$current_date-$current_nao/nao.info
echo "Name=$current_nao_name" >> /media/brainwasher/$current_date-$current_nao/nao.info
echo "Number=$current_nao_number" >> /media/brainwasher/$current_date-$current_nao/nao.info
echo "$current_nao_player" >> /media/brainwasher/$current_date-$current_nao/nao.info

# find log files and copy them to the created directory
#find -L /tmp -type d -name media -prune -o -name "*.log" -exec cp {} /media/brainwasher/$current_date-$current_nao \;
# find log files, create MD5 hashes and copy everything to the created directory
for f in $(find -L /tmp -type d -name media -prune -o -name "*.log")
do
	md5sum $f | sed -e "s/\/tmp\///g" > "$f.md5"
	cp "$f.md5" $f /media/brainwasher/$current_date-$current_nao/
	check_for_errors "Brainwasher:ERROR copying $f"
done

# copy the config directory
cp -r /home/nao/naoqi/Config /media/brainwasher/$current_date-$current_nao
check_for_errors "Brainwasher:ERROR copying config"

# create dump folder and log errors
mkdir -p /media/brainwasher/$current_date-$current_nao/dumps 2> $errorFile
logger -f $errorFile

# find and copy trace dump files since boot and log errors
find /home/nao -maxdepth 1 -type f -mmin -$current_boot_time -iname "trace.dump.*" -exec cp {} /media/brainwasher/$current_date-$current_nao/dumps/ \; 2> $errorFile
logger -f $errorFile

# create whistle folder and log errors
mkdir -p /media/brainwasher/$current_date-$current_nao/whistle 2> $errorFile
logger -f $errorFile

# find and copy whistle raw files and log errors
find /tmp/ -maxdepth 1 -type f -iname "capture_*.raw" -exec cp {} /media/brainwasher/$current_date-$current_nao/whistle/ \; 2> $errorFile
logger -f $errorFile


# remove error log file
rm $errorFile

# make sure, everything is written
sync

# needed to play sound before starting naoth! otherwise the sound could get "lost" (no sound)
sudo -u nao /usr/bin/paplay /home/nao/naoqi/Media/nicknacknuck.wav

logger "Brainwasher:END, starting naoth"

naoth start
