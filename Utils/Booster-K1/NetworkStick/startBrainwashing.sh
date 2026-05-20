#!/bin/bash

NAME=networkstick source buloginit

info "Beginning"

WLAN_SSID=NAONET
WLAN_PW=a1b0a1b0a1
WLAN_IP=10.0.4
WLAN_SUBNET_MASK=16

# TODO: how to get this on the booster
NR=41

IFNAME=wlP1p1s0
if [[ $(ip a | grep $IFNAME) == "" ]]; then
    error "Wifi interface name changed/ does not exist! Exiting."
    exit 1
fi

info "Setting SSID=$WLAN_SSID, PW=$WLAN_PW, IP=$WLAN_IP.$NR/$WLAN_SUBNET_MASK, IF=$IFNAME"

# NOTE: NO NOT change the name of the connection file
cat - <<EOF >/etc/NetworkManager/system-connections/WIFI.nmconnection
[connection]
id=WIFI
uuid=975cebb3-4286-40fb-8856-80336335e3b1
type=wifi
interface-name=$IFNAME
autoconnect=true

[wifi]
mode=infrastructure
ssid=$WLAN_SSID
cloned-mac-address=permanent

[wifi-security]
key-mgmt=wpa-psk
psk=$WLAN_PW

[ipv4]
address1=$WLAN_IP.$NR/$WLAN_SUBNET_MASK
dns=$WLAN_IP.1;1.1.1.1;8.8.8.8;
gateway=$WLAN_IP.1
method=manual

[ipv6]
addr-gen-mode=stable-privacy
method=disabled

[proxy]
EOF

# To avoid "failed to load connection: File permissions (100644) are insecure"
chown root:root /etc/NetworkManager/system-connections/*
chmod 600 /etc/NetworkManager/system-connections/*

systemctl restart NetworkManager
nmcli connection modify 975cebb3-4286-40fb-8856-80336335e3b1 wifi.cloned-mac-address permanent

info "Done"
