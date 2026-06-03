#!/bin/bash

# udev will pass the partion name
PARTITION=$1
DEVICE="/dev/$PARTITION"
MOUNT_POINT="/mnt/usb_trigger"
TARGET_SCRIPT="$MOUNT_POINT/startBrainwashing.sh"

if [ -z "$PARTITION" ]; then
    exit 1
fi

sleep 1

mkdir -p "$MOUNT_POINT"

echo "$DEVICE $MOUNT_POINT" 

mount "$DEVICE" "$MOUNT_POINT"

if [ -f "$TARGET_SCRIPT" ]; then
    chmod +x "$TARGET_SCRIPT"

    "$TARGET_SCRIPT" > $MOUNT_POINT/get_new_network.log 2>&1
fi

umount "$MOUNT_POINT"