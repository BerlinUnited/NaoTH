#!/bin/bash
# set volume to 88%
sudo -u nao pactl set-sink-mute 0 false
sudo -u nao pactl set-sink-volume 0 88%
# play initial sound
sudo -u nao /usr/bin/paplay /home/nao/naoqi/Media/usb_start.wav

nao stop

# copy function
copy(){
  local from="$1"
  local to="$2"
  local owner="$3"
  local rights="$4"

  if [ -f ${from} ]
  then
	  echo "copy ${from} to ${to}";

    # backup
    if [ -f ${to} ]
    then
      mv "${to}" "${to}.bak";
    fi

    # copy the file
	  cp -f ${from} ${to};

    #set correct rights
    echo "set owner ${owner}";
    chown ${owner}:${owner} ${to};
    echo "set rights ${rights}";
    chmod ${rights} ${to};

    return 0
  else
	  echo "missing file ${from}";
    return 1
  fi
}

copy ./autoload.ini /etc/naoqi/autoload.ini root 644

nao start
