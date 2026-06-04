#!/bin/bash
set -x

SSID="NAONET_5G"
NETWORK_NAME="NAONET_5G"

# play sound
/usr/bin/paplay $HOME/naoqi/Media/usb_start.wav"

echo "Generate network configuration";

sudo tee /etc/NetworkManager/system-connections/$NETWORK_NAME.nmconnection > /dev/null <<EOF
[connection]
id=$NETWORK_NAME
type=wifi
interface-name=wlP1p1s0
autoconnect-priority=1

[wifi]
mode=infrastructure
ssid=$SSID

[wifi-security]
auth-alg=open
key-mgmt=wpa-psk
psk=a1b0a1b0a1

[ipv4]
method=manual
addresses=10.0.4.41/16
gateway=10.0.4.1
dns=8.8.8.8;1.1.1.1;

[ipv6]
addr-gen-mode=stable-privacy
method=auto

[proxy]
EOF

sudo chmod 600 /etc/NetworkManager/system-connections/$NETWORK_NAME.nmconnection


sudo nmcli connection reload
# sudo nmcli device disconnect wlP1p1s0 # if we encounter errors we might need to do this
sudo nmcli connection up $NETWORK_NAME