#!/bin/bash

# switch between scripts to use for Nao's up to version 5 or Nao's version 6
if [ -f "/opt/aldebaran/bin/lola" ] | [ -f "/usr/bin/lola" ]; then

	echo "Init script for Nao V6"
	
    export BACKUP_DIRECTORY="/home/nao/backup_$(date +"%y%m%d-%H%M")"
    export DEPLOY_DIRECTORY=`pwd`"/deploy"

	chmod +x brainwash_v6.sh
	./brainwash_v6.sh

else
	echo "Init script for Nao V5 or older"	

    cd deploy/v3v4v5
	chmod +x init_env.sh
	./init_env.sh
