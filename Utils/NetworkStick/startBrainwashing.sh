# set volume to 88%
su nao -c "/usr/bin/pactl set-sink-mute 0 false"
su nao -c "/usr/bin/pactl set-sink-volume 0 88%"
# also set the recording volume
# 1. set in simple mode with alsa mixer to make sure it is in sync for all channels
su nao -c "/usr/bin/amixer sset 'Capture',0 90%"
# 2. set with pulseaudio (now both channels are set) to make sure the changes are persistent
su nao -c "/usr/bin/pactl set-source-mute 1 false"
su nao -c "/usr/bin/pactl set-source-volume 1 90%"


# play initial sound
su nao -c "/usr/bin/paplay /home/nao/naoqi/Media/usb_start.wav"

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

if [ ! -f "/opt/aldebaran/bin/lola" ] && [ ! -f "/usr/bin/lola" ]; then

  # determine the head number, e.g., hostname=Nao82 => N=82
  N=$(cat /etc/hostname | grep -Eo "[0-9]{2}")
  rm -f ./etc/conf.d/net
  echo "config_wlan0=\"10.0.4.$N netmask 255.255.255.0 brd 10.0.4.255\"" > ./etc/conf.d/net
  echo "config_eth0=\"192.168.13.$N netmask 255.255.255.0 brd 192.168.13.255\"" >> ./etc/conf.d/net
  echo "wpa_supplicant_wlan0=\"-Dnl80211\"" >> ./etc/conf.d/net

  # WLAN Encryption
  copy ./etc/wpa_supplicant/wpa_supplicant.conf /etc/wpa_supplicant/wpa_supplicant.conf root 644

  # Network IP Adresses
  copy ./etc/conf.d/net /etc/conf.d/net root 644

  # restart the network
  /etc/init.d/net.eth0 restart
  /etc/init.d/net.wlan0 restart

else

  WIFI_STATE=$( ifconfig | grep -o wlan0)
  if [ -z $WIFI_STATE ]; then
    connmanctl enable wifi
    sleep 0.1
  fi
  connmanctl scan wifi
  CONNMAN_SERVICES=$(connmanctl services)

  ETH0_MAC=$(cat /sys/class/net/eth0/address | sed -e 's/://g')
  WLAN0_MAC=$(cat /sys/class/net/wlan0/address | sed -e 's/://g')

  echo "Setting up wifi configuration for wlan0 (${WLAN0_MAC})"
  sed -i -e "s/__NAO__/${NAO_NUMBER}/g" $DEPLOY_DIRECTORY/v6/var/lib/connman/wifi.config
  deployFile "/var/lib/connman/wifi.config" "root" "644" "v6"

  WIFI_NETWORKS=$(cat /var/lib/connman/wifi.config | grep "Name =" | sed -e "s/ //g" | sed -e "s/Name=//g")
  for wifi in $WIFI_NETWORKS; do
    if [ ! $wifi == "wifi" ]; then
      # echo "$CONNMAN_SERVICES" | grep "SPL_A " | grep -o wifi.*
      service=$(echo "$CONNMAN_SERVICES" | grep "$wifi " | grep -o wifi.*)
      if [ ! -z $service ]; then
        echo "Disabling autoconnect on wifi network $wifi"
        # echo $service
        connmanctl config ${service} --autoconnect off
        # echo "connmanctl config ${service} --autoconnect off"
      else
          echo "Wifi network $wifi currently not available"
      fi
    fi
  done

  echo "Setting ip of eth0 (${ETH0_MAC})"
  connmanctl config ethernet_${ETH0_MAC}_cable --ipv4 manual 192.168.13.${NAO_NUMBER} 255.255.255.0

fi

naoth restart





