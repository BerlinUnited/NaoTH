#!/bin/sh

# CONFIG:
# (IPs in form "xxx.xxx.xxx"!)

# some defaults:

# # NAONET:
# NETWORK_WLAN_SSID="NAONET"
# NETWORK_WLAN_PW="a1b0a1b0a1"
# NETWORK_WLAN_IP="10.0.4"
# NETWORK_WLAN_MASK="255.255.255.0"
# NETWORK_WLAN_BROADCAST="10.0.4.255"

# # SPL_B:
# NETWORK_WLAN_SSID="SPL_A"
# NETWORK_WLAN_PW="Nao?!Nao?!"
# NETWORK_WLAN_IP="10.0.4"
# NETWORK_WLAN_MASK="255.255.0.0"
# NETWORK_WLAN_BROADCAST="10.0.255.255"

# CUSTOM:
NETWORK_WLAN_SSID="NAONET"
NETWORK_WLAN_PW="a1b0a1b0a1"
NETWORK_WLAN_IP="10.0.4"
NETWORK_WLAN_MASK="255.255.255.0"
NETWORK_WLAN_BROADCAST="10.0.4.255"


NETWORK_ETH_IP="192.168.13"
NETWORK_ETH_MASK="255.255.255.0"
NETWORK_ETH_BROADCAST="192.168.13.255"


N=$(cat /etc/hostname | grep -Eo "[0-9]{2}" | head -n 1 )
NETWORK_WLAN_MAC=$(cat /sys/class/net/wlan0/address | sed -e 's/://g')
NETWORK_WLAN_MAC_FULL=$(cat /sys/class/net/wlan0/address | tr a-z A-Z)
NETWORK_ETH_MAC=$(cat /sys/class/net/eth0/address | sed -e 's/://g')

FINAL_WLAN_IP="$NETWORK_WLAN_IP.$N"
FINAL_ETH_IP="$NETWORK_ETH_IP.$N"

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

# copy files and custom own them, preserving old as .old
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

# generate linux network configuration
gen_conf_d_net(){
  echo "generating $1/net . . ."
  rm -rf ./net

  cat << EOF > ./net
config_wlan0="$FINAL_WLAN_IP netmask $NETWORK_WLAN_MASK broadcast $NETWORK_WLAN_BROADCAST"
config_eth0="$FINAL_ETH_IP netmask $NETWORK_ETH_MASK broadcast $NETWORK_ETH_BROADCAST"
wpa_supplicant_wlan0="-Dnl80211"
EOF

  copy ./net "$1/net" root 644
}

# generate wpa_supplicant configuration
gen_wpa_supplicant(){
  echo "generating $1/wpa_supplicant.conf . . ."
  rm -rf ./wpa_supplicant.conf

  cat << EOF > ./wpa_supplicant.conf
ctrl_interface=/var/run/wpa_supplicant
ctrl_interface_group=0
ap_scan=1

network={
  ssid="$NETWORK_WLAN_SSID"
  key_mgmt=WPA-PSK
  psk="$NETWORK_WLAN_PW"
  priority=5
}
EOF

  copy ./wpa_supplicant.conf "$1/wpa_supplicant.conf" root 644
}

# ---------- network setup ---------- #


# NAOv5
if [ ! -f "/opt/aldebaran/bin/lola" ] && [ ! -f "/usr/bin/lola" ]; then
  echo "running NAOv5 setup"

  gen_conf_d_net "/etc/conf.d/"
  gen_wpa_supplicant "/etc/wpa_supplicant/"

  /etc/init.d/net.eth0 restart
  /etc/init.d/net.wlan0 restart

# NAOv6
else
  echo "running NAOv6 setup"

  gen_conf_d_net "/home/nao/.config/"
  gen_wpa_supplicant "/home/nao/.config/"

  systemctl restart net.eth0
  systemctl restart net.wlan0
fi

# restart 
echo "restarting . . ."
naoth restart
