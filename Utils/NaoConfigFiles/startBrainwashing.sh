#!/bin/bash

# ==================== var definition ====================

export BACKUP_DIRECTORY="/home/nao/backup_$(date +"%y%m%d-%H%M")"
export DEPLOY_DIRECTORY=`pwd`"/deploy"

export NETWORK_WLAN_SSID="NAONET"
export NETWORK_WLAN_PW="a1b0a1b0a1"
export NETWORK_WLAN_IP="10.0.4"
export NETWORK_WLAN_MASK="255.255.255.0"
export NETWORK_WLAN_BROADCAST="10.0.4.255"

export NETWORK_ETH_IP="10.1.4"
export NETWORK_ETH_MASK="255.255.255.0"
export NETWORK_ETH_BROADCAST="10.0.4.255"

# switch between scripts to use for Nao's up to version 5 or Nao's version 6
if [ -f "/opt/aldebaran/bin/lola" ] | [ -f "/usr/bin/lola" ]; then

	echo "Nao V6"
	
	chmod +x brainwash_v6.sh
	nohup ./brainwash_v6.sh &>/dev/null &

else
	echo "Nao V5 or older"	

	chmod +x brainwash_v3v4v5.sh
	nohup ./brainwash_v3v4v5.sh &>/dev/null &
fi
