#!/bin/bash

sudo -u nao /usr/bin/paplay /home/nao/naoqi/Media/usb_stop.wav

# set file vars
infoFile="/home/nao/Config/nao.info"
errorFile="/home/nao/brainwasher.log"

# set vars
current_date=$(date +"%y%m%d-%H%M")
current_nao=$(sed -n "2p" $infoFile)
current_boot_time=$(</proc/uptime awk '{printf "%d", $1 / 60}')

# write to systemlog
logger "Brainwasher:start $current_date, $current_nao"

mkdir $current_date-$current_nao

find -L /tmp -type d -name media -prune -o -name "*.log" -exec cp {} /media/brainwasher/$current_date-$current_nao \;

cp -r /home/nao/naoqi/Config /media/brainwasher/$current_date-$current_nao

# create dump folder and log errors
mkdir -p /media/brainwasher/$current_date-$current_nao/dumps 2> $errorFile
logger -f $errorFile

# find and copy trace dump files since boot and log errors
find /home/nao -maxdepth 1 -type f -mmin -$current_boot_time -iname "trace.dump.*" -exec cp {} /media/brainwasher/$current_date-$current_nao/dumps/ \; 2> $errorFile
logger -f $errorFile

# remove error log file
rm $errorFile

logger "Brainwasher:end"
