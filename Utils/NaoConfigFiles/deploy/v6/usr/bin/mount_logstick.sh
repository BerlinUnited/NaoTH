#!/bin/bash

# redirect all the output to the logger
exec 1> >(logger -s -t mount_logstick) 2>&1

# create mount point
mount_point=/home/logstick
mkdir -p "${mount_point}"

# mount usb drive
if ! mount /dev/logstick "${mount_point}"; then
  logger -t mount_logstick "ERROR: usb unknown mount error"
  su nao -c "/usr/bin/paplay /home/nao/naoqi/Media/error_usb_unknown_mount_error.wav"
  exit 0
fi

chown --recursive nao:nao "${mount_point}"

su nao -c "/usr/bin/paplay /home/nao/naoqi/Media/usb_start.wav"
