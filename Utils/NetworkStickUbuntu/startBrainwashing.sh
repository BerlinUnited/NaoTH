#!/bin/sh

# CONFIG:
# (IPs in form "xxx.xxx.xxx"!)

# some defaults:

# # NAONET:
# NETWORK_WLAN_SSID="NAONET"
# NETWORK_WLAN_PW="a1b0a1b0a1"
# NETWORK_WLAN_IP="10.0.4"
# NETWORK_WLAN_MASK="255.255.0.0"
# NETWORK_WLAN_BROADCAST="10.0.255.255"

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
NETWORK_WLAN_MASK="255.255.0.0"
NETWORK_WLAN_BROADCAST="10.0.255.255"


NETWORK_ETH_IP="192.168.13"
NETWORK_ETH_MASK="255.255.255.0"
NETWORK_ETH_BROADCAST="192.168.13.255"


HEAD_ID=\$(cat /sys/qi/head_id)
N=\$(grep "\$HEAD_ID" /opt/naoth/robots.cfg | sed 's/.*=\\([0-9]\+\\)/\1/')
if [[ -z "\$N" ]]; then N=99; fi # fallback number 99 !

# hostname
hostnamectl set-hostname "nao\$N"

echo "Generate network configuration";
WLAN_SSID="NAONET"
WLAN_PW="a1b0a1b0a1"
WLAN_IP="10.0.4"
WLAN_SUBNET_MASK="16"

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
        - \${LAN_IP}.\${N}/\${LAN_SUBNET_MASK}
      dhcp4: false
      dhcp6: false
      gateway4: \${LAN_IP}.1
  wifis:
    wlan0:
      optional: true
      access-points:
        "\${WLAN_SSID}":
          password: "\${WLAN_PW}"
      addresses:
        - \${WLAN_IP}.\${N}/\${WLAN_SUBNET_MASK}
      nameservers:
        addresses: [8.8.8.8, \${WLAN_IP}.1]
      dhcp4: false
      dhcp6: false
      gateway4: \${WLAN_IP}.1
EOF

netplan apply

# restart 
echo "restarting . . ."
naoth restart
