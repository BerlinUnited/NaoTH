#!/bin/bash
set -x

# play sound
su nao -c "/usr/bin/paplay /home/nao/naoqi/Media/usb_start.wav"

HEAD_ID=$(cat /sys/qi/head_id)
N=$(grep "$HEAD_ID" /opt/naoth/robots.cfg | sed 's/.*=\([0-9]\+\)/\1/')
if [[ -z "$N" ]]; then N=99; fi # fallback number 99 !

echo "Generate network configuration";
WLAN_SSID="NAONET"
WLAN_PW="a1b0a1b0a1"
WLAN_IP="10.0.4"
WLAN_SUBNET_MASK="16"

# e.g. for a competition set the following
# WLAN_SSID="SPL_A"
# WLAN_PW="Nao?!Nao?!"

LAN_IP="192.168.13"
LAN_SUBNET_MASK="16"

cat - <<EOF > /etc/netplan/default.yaml
network:
  version: 2
  renderer: networkd
  ethernets:
    eth0:
      optional: true
      addresses:
        - $LAN_IP.$N/$LAN_SUBNET_MASK
      dhcp4: false
      dhcp6: false
      gateway4: $LAN_IP.1
  wifis:
    wlan0:
      optional: true
      access-points:
        "$WLAN_SSID":
          password: "$WLAN_PW"
      addresses:
        - $WLAN_IP.$N/$WLAN_SUBNET_MASK
      nameservers:
        addresses: [8.8.8.8, $WLAN_IP.1]
      dhcp4: false
      dhcp6: false
      gateway4: $WLAN_IP.1
EOF

netplan apply

