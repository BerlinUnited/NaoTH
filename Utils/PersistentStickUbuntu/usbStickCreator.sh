#!/bin/bash

# formatting must be done with gnome-disks

# run this script as root

# !!! do not run if any of your drives are already one of /dev/sd* !!!

set -x
set -e

cd /dev

for f in sd?1; do
  mkdir ~/$f
  mount $f ~/$f
  rm -r ~/$f/*
  cp /home/anton/mobrob/naoth-2020/Utils/PersistentStickUbuntu/startBrainwashing.sh ~/$f
  echo "This file is important." > ~/$f/logstick.info
  umount ~/$f
  rmdir ~/$f
  eject $f
done

sync
