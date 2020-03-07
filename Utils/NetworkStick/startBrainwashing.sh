# CONFIG:
# (IPs in form "xxx.xxx.xxx"!)

# some defaults:
# NAONET:
# NETWORK_WLAN_SSID="NAONET"
# NETWORK_WLAN_PW="a1b0a1b0a1"
# NETWORK_WLAN_IP="10.0.4"
# NETWORK_WLAN_MASK="255.255.0.0"
# NETWORK_WLAN_BRIDGE="10.0.4.255"


NETWORK_WLAN_SSID="NAONET"
NETWORK_WLAN_PW="a1b0a1b0a1"
NETWORK_WLAN_IP="10.0.4"
NETWORK_WLAN_MASK="255.255.0.0"
NETWORK_WLAN_BRIDGE="10.0.4.255"

NETWORK_ETH_IP="192.168.13"
NETWORK_ETH_MASK="255.255.255.0"
NETWORK_ETH_BRIDGE="192.168.13.255"


N=$(cat /etc/hostname | grep -Eo "[0-9]{2}")
NETWORK_WLAN_MAC=$(cat /sys/class/net/wlan0/address | sed -e 's/://g')
NETWORK_WLAN_MAC_FULL=$(cat /sys/class/net/wlan0/address | tr a-z A-Z)
NETWORK_ETH_MAC=$(cat /sys/class/net/eth0/address | sed -e 's/://g')

NETWORK_WLAN_IP="$NETWORK_WLAN_IP.$N"
NETWORK_ETH_IP="$NETWORK_ETH_IP.$N"

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

copy(){
  local from="$1"
  local to="$2"
  local owner="$3"
  local rights="$4"

  if [ -f ${from} ]
  then
	  echo "copying ${from} to ${to}";

    # backup
    if [ -f ${to} ]
    then
      mv "${to}" "${to}.bak";
    fi

    # copy the file
	  cp -f ${from} ${to};

    #set correct rights
    echo "setting owner ${owner}";
    chown ${owner}:${owner} ${to};
    echo "setting rights ${rights}";
    chmod ${rights} ${to};

    return 0
  else
	  echo "missing file ${from}";
    return 1
  fi
}

gen_conf_d_net(){
  echo "generating /etc/conf.d/net . . ."
  rm -rf ./net

  echo "
  config_wlan0=\"$NETWORK_WLAN_IP netmask $NETWORK_WLAN_MASK brd $NETWORK_WLAN_BRIDGE\"
  config_eth0=\"$NETWORK_ETH_IP netmask $NETWORK_ETH_MASK  brd $NETWORK_ETH_BRIDGE\"
  wpa_supplicant_wlan0=\"-Dnl80211\"" > ./net

  copy ./net /etc/conf.d/net root 644
}

gen_wpa_supplicant(){
  echo "generating /etc/wpa_supplicant.conf . . ."
  rm -rf ./wpa_supplicant.conf

  echo "
  ctrl_interface=/var/run/wpa_supplicant
  ctrl_interface_group=0
  ap_scan=1

  network={
    ssid=\"$NETWORK_WLAN_SSID\"
    key_mgmt=WPA_PSK
    psk=\"$NETWORK_WLAN_PW\"
    priority=5
  }" > ./wpa_supplicant.conf

  copy ./wpa_supplicant.conf /etc/wpa_supplicant.conf root 644
}

gen_connman_config(){
  echo "generating /var/lib/connman/wifi.config . . ."
  rm -rf ./wifi.config

  echo "
  [global]
  name=wifi
  Description=wifi network settings

  [service_$NETWORK_WLAN_SSID]
  Type = wifi
  Name = $NETWORK_WLAN_SSID
  Passphrase = $NETWORK_WLAN_PW
  IPv4 = $NETWORK_WLAN_IP/$NETWORK_WLAN_MASK/0.0.0.0
  MAC = $NETWORK_WLAN_MAC_FULL" > ./wifi.config

  copy ./wifi.config /var/lib/connman/wifi.config root 644
}







# ---------- network setup ---------- #

# NAOv5
if [ ! -f "/opt/aldebaran/bin/lola" ] && [ ! -f "/usr/bin/lola" ]; then
  echo "running NAOv5 setup"

  gen_conf_d_net
  gen_wpa_supplicant

  /etc/init.d/net.eth0 restart
  /etc/init.d/net.wlan0 restart

# NAOv6
else
  echo "running NAOv6 setup"

  WIFI_STATE=$( ifconfig | grep --o wlan0)
  if [ -z $WIFI_STATE ]; then
    connmanctl enable wifi
    sleep 0.1
  fi
  connmanctl scan wifi

  gen_connman_config

  service=$(connmanctl services | grep "$NETWORK_WLAN_SSID " | grep -o wifi.*)

  # check if service is connected
  if [ ! -z $service ]; then
    connmanctl config ${service} --autoconnect on
    connmanctl connect ${service}
  else
    echo "wifi network $NETWORK_WIFI_SSID currently not available"
  fi

  echo "setting ip of eth0 (${NETWORK_ETH_MAC})"
  connmanctl config ethernet_${NETWORK_ETH_MAC}_cable --ipv4 manual $NETWORK_ETH_IP $NETWORK_ETH_MASK 0.0.0.0
fi

echo "restarting . . ."
naoth restart
