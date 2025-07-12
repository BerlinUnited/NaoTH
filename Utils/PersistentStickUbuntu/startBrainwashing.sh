#!/bin/bash

#INFO: file is executed via root on the nao!
#		- plays "usb_stop.wav" in the beginning
#		- plays "1.wav" if a copy-error occurred (game.log, Config/)
#		- plays "nicknacknuck.wav" at the end
# IMPORTANT: Only for Naos with Ubuntu

# write to systemlog
logger "Brainwasher:START mounting persistent log stick"

# play sound
su nao -c "/usr/bin/paplay /home/nao/naoqi/Media/usb_start.wav"

# write previous volume to systemlog
current_volume=$(su nao -c "pactl list sinks | grep Volume | xargs")
logger "Brainwasher:$current_volume" # ... ok, but why?

# set volume to 88%
errorFile="/home/nao/brainwasher.log"
su nao -c "/usr/bin/pactl set-sink-mute 0 false 2> $errorFile"
su nao -c "/usr/bin/pactl set-sink-volume 0 88% 2>> $errorFile"
logger -f $errorFile
# remove error log file
rm $errorFile

# logger "Brainwasher:sleeping until logstick is removed"
while [[ -e /dev/sda1 ]]; do
	sleep 1
done

# needed to play sound before starting naoth! otherwise the sound could get "lost" (no sound)
su nao  -c "/usr/bin/paplay /home/nao/naoqi/Media/finished_collecting_logs.wav"

logger "Brainwasher:END mounted persistent log stick"
