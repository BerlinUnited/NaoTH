#!/bin/bash

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

# -------- network dependent --------

# WLAN Encryption
copy ./etc/wpa_supplicant/wpa_supplicant.conf /etc/wpa_supplicant/wpa_supplicant.conf root 644

# Network IP Adresses
copy ./etc/conf.d/net /etc/conf.d/net root 644

# restart the network
/etc/init.d/net.eth0 restart
/etc/init.d/net.wlan0 restart





