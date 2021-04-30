#!/bin/bash

# ==================== var definition ====================

export BACKUP_DIRECTORY="/home/nao/backup_$(date +"%y%m%d-%H%M")"
export DEPLOY_DIRECTORY=`pwd`"/deploy"

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
