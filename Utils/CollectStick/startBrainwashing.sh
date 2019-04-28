#!/bin/bash

#INFO: file is executed via root on the nao!
#		- plays "usb_stop.wav" in the beginning
#		- plays "1.wav" if a copy-error occurred (game.log, Config/)
#		- plays "nicknacknuck.wav" at the end

# define functions
check_for_errors() {
	if [ "$?" -ne 0 ]
	then
		sudo -u nao /usr/bin/paplay /home/nao/naoqi/Media/error_while_collecting_logs.wav
		# if argument is available - write to systemlog
		if [ ! -z "$1" ]
		then
			logger "$1"
		fi
	fi
}

exec_cmd_and_return_or_default() {
	result=$(eval "$1" 2>&1)
	if [ "$?" -ne 0 ]; then
		echo "$2"
		exit 1
	fi
	echo $result
}


# set file vars
infoFile="/home/nao/Config/nao.info"
errorFile="/home/nao/brainwasher.log"

# set vars
current_date=$(date +"%y%m%d-%H%M")
current_nao=$(sed -n "2p" $infoFile)
current_nao_name=$(cat /etc/hostname) # get the name, eg. "nao96"
current_nao_number=$(cat /etc/hostname | grep -Eo "[0-9]{2}") # get the number, e.g. "96"
current_nao_player=$(exec_cmd_and_return_or_default 'grep "\[PlayerInfo\] playerNumber" /var/log/messages | tail -1 | grep -o "playerNumber.*"' 'playerNumber: UNKNOWN')

current_boot_time=$(</proc/uptime awk '{printf "%d", $1 / 60}')

current_compile_branch=$(exec_cmd_and_return_or_default 'grep "Branch path" /var/log/messages | tail -1 | grep -o "Branch path.*"' 'Branch path: UNKNOWN')
current_compile_revision=$(exec_cmd_and_return_or_default 'grep "Revision number" /var/log/messages | tail -1 | grep -o "Revision number.*"' 'Revision number: UNKNOWN')
current_compile_time=$(exec_cmd_and_return_or_default 'grep "NaoTH compiled on" /var/log/messages | tail -1 | grep -o "NaoTH compiled on.*"' 'NaoTH compiled on: UNKNOWN')
current_compile_owner=$(exec_cmd_and_return_or_default 'grep "Owner" /var/log/messages | tail -1 | grep -o "Owner.*"' 'Owner: UNKNOWN')

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
dir_name=$current_date-$current_nao
target_path=/media/brainwasher/$dir_name
mkdir $dir_name

# copy info file of the nao
cp $infoFile $target_path/nao.info
echo "Name=$current_nao_name" >> $target_path/nao.info
echo "Number=$current_nao_number" >> $target_path/nao.info
echo "$current_nao_player" >> $target_path/nao.info
echo "$current_compile_branch" >> $target_path/nao.info
echo "$current_compile_revision" >> $target_path/nao.info
echo "$current_compile_time" >> $target_path/nao.info
echo "$current_compile_owner" >> $target_path/nao.info

# find log files and copy them to the created directory
#find -L /tmp -type d -name media -prune -o -name "*.log" -exec cp {} /media/brainwasher/$current_date-$current_nao \;
# find log files, create MD5 hashes and copy everything to the created directory
for f in $(find -L /tmp -type d -name media -prune -o -name "*.log")
do
	md5sum $f | sed -e "s/\/tmp\///g" > "$f.md5"
	cp "$f.md5" $f $target_path/
	check_for_errors "Brainwasher:ERROR copying $f"
done

# copy the config directory
cd /home/nao/naoqi
zip -q -r -0 $target_path/config.zip Config
cd -
check_for_errors "Brainwasher:ERROR copying config"

# copy logs of naoth binary (std::out/::err) and clear them afterwards
cp "/var/log/naoth.log" "/var/log/naoth_err.log" $target_path/
check_for_errors "Brainwasher:ERROR copying /var/log/naoth.log"
> /var/log/naoth.log
> /var/log/naoth_err.log

# find and copy trace dump files since boot and log errors
find /home/nao -maxdepth 1 -type f -mmin -$current_boot_time -iname "trace.dump.*" -exec zip -q -0 $target_path/dumps.zip {} + 2> $errorFile
logger -f $errorFile
# if no error occurred, we can savely delete all trace dump files, which were previously copied
if [ ! -s "$errorFile" ]; then
  find /home/nao -maxdepth 1 -type f -mmin -$current_boot_time -iname "trace.dump.*" -exec rm {} \; 2> $errorFile
  logger -f $errorFile
fi

# find and copy whistle raw files and log errors
find /tmp/ -maxdepth 1 -type f -iname "capture_*.raw" -exec zip -q -0 $target_path/whistle.zip {} + 2> $errorFile
logger -f $errorFile
# if no error occurred, we can savely delete all whistle files
if [ ! -s "$errorFile" ]; then
  find /tmp/ -maxdepth 1 -type f -iname "capture_*.raw" -exec rm {} \; 2> $errorFile
  logger -f $errorFile
fi

# remove error log file
rm $errorFile

# make sure, everything is written
sync

# needed to play sound before starting naoth! otherwise the sound could get "lost" (no sound)
sudo -u nao /usr/bin/paplay /home/nao/naoqi/Media/finished_collecting_logs.wav

logger "Brainwasher:END, starting naoth"

naoth start
