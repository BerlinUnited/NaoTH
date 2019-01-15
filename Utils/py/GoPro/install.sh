#!/bin/bash

# determine path
GOPRO_HOME=$(dirname "$(readlink -f -- "$0")")
DHCP_CONFIG="/etc/dhcpcd.conf"
REBOOT=false

#check if root first
if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root"
   exit 1
fi

############################################################################################
# FUNCTIONS
############################################################################################

check_dependencies() {
	# check python v3
	which python3 > /dev/null
	if [ $? != 0 ]; then
		echo "Python 3 is required! Please install first."
		return 1
	fi
	# check python module
	python3 -c "import RPi.GPIO" 2> /dev/null
	if [ $? != 0 ]; then
		echo "Python module 'RPi.GPIO' is required! Please install first (eg. 'sudo apt-get install python3-rpi.gpio')."
		return 2
	fi
	# everything 'ok'
	return 0
}

setup_static_ip() {
	# already configured?
	grep $DHCP_CONFIG -e "^\s*static ip_address.*" > /dev/null
	if [[ $? == 0 ]]; then
		read -p "There is already a static ip address configured! Replace existing? [y|N]: " -r INPUT
		if [[ $INPUT != "y" && $INPUT != "Y" ]]; then
			return 0
		fi
	fi

	# set defaults
	IF="eth0"
	IP="10.0.4.99/16"
	R="10.0.0.1"
	DNS="10.0.0.1 8.8.8.8"

	# which interface?
	read -p "interface [$IF]: " -r INPUT
	if [[ ! -z $INPUT ]]; then
		IF=$INPUT
	fi

	# which ip address?
	read -p "ip address [$IP]: " -r INPUT
	if [[ ! -z $INPUT ]]; then
		until [[ -z $INPUT || $INPUT =~ ^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}/[0-9]{1,3}$ ]]; do
			echo "invalid format: a.b.c.d/m"
			read -p "ip address [$IP]: " -r INPUT
		done
		if [[ ! -z $INPUT ]]; then
			IP=$INPUT
		fi
	fi
	
	# which router ip address?
	read -p "router address [$R]: " -r INPUT
	if [[ ! -z $INPUT ]]; then
		until [[ -z $INPUT || $INPUT =~ ^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$ ]]; do
			echo "invalid format: a.b.c.d"
			read -p "router address [$R]: " -r INPUT
		done
		if [[ ! -z $INPUT ]]; then
			R=$INPUT
		fi
	fi

	# which dns ip address?
	read -p "dns address [$DNS]: " -r INPUT
	if [[ ! -z $INPUT ]]; then
		until [[ -z $INPUT || $INPUT =~ ^([0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}[ ]*)+$ ]]; do
			echo "invalid format: a.b.c.d"
			read -p "dns address [$DNS]: " -r INPUT
		done
		if [[ ! -z $INPUT ]]; then
			DNS=$INPUT
		fi
	fi
	
	# remove 'old' configuration
	remove_static_ip

	# set the new configuration
	echo -e "interface $IF\nstatic ip_address=$IP\nstatic routers=$R\nstatic domain_name_servers=$DNS\n" >> $DHCP_CONFIG
	REBOOT=true
}

setup_hostname() {
    if [[ "$1" == "" ]]; then
        NAME=`cat /etc/hostname`
        # ask for hostname
        read -p "hostname [$NAME]: " -r INPUT
        if [[ ! -z $INPUT ]]; then
            NAME=$INPUT
        fi
    else
        NAME="$1"
    fi
	echo -e "$NAME" > /etc/hostname
	REBOOT=true
}

remove_static_ip() {
	# remove existing configuration
	sed -i '/^\s*interface/ d' $DHCP_CONFIG
	sed -i '/^\s*static ip_address/ d' $DHCP_CONFIG
	sed -i '/^\s*static routers/ d' $DHCP_CONFIG
	sed -i '/^\s*static domain_name_servers/ d' $DHCP_CONFIG
}

stop_service() {
	# helper for stopping the services
	systemctl -q status $1 > /dev/null
	if [ $? == 0 ]; then
		echo "stopping $1"
		systemctl stop $1 > /dev/null
	fi
}

install() {
	echo "installing ..."
	
	check_dependencies
    if [[ $? != 0 ]]; then
    	exit $?
    fi

	stop_service gopro
	stop_service led

	# copy the executable script
	cp $GOPRO_HOME/gopro /usr/bin/
	cp $GOPRO_HOME/led /usr/bin/

	# replace the default executable path
	sed -i "s!/home/pi/GoPro!$GOPRO_HOME!" /usr/bin/gopro
	sed -i "s!/home/pi/GoPro!$GOPRO_HOME!" /usr/bin/led

	# make scripts executable
	chmod +x /usr/bin/gopro
	chmod +x /usr/bin/led
	chmod +x $GOPRO_HOME/main.py
	chmod +x $GOPRO_HOME/LEDServer.py

	# install the service files
	cp $GOPRO_HOME/gopro.service /lib/systemd/system/
	cp $GOPRO_HOME/led.service /lib/systemd/system/
	# make them executable
	chmod 644 /lib/systemd/system/gopro.service
	chmod 644 /lib/systemd/system/led.service
	
	# update services
	systemctl daemon-reload
	systemctl enable gopro.service
	systemctl enable led.service

	systemctl start led
	systemctl start gopro

	read -p "Setup static ip address? [y|N]: " -r INPUT
	if [[ $INPUT == "y" || $INPUT == "Y" ]]; then
		setup_static_ip
	fi

	read -p "Setup (unique) hostname? [y|N]: " -r INPUT
	if [[ $INPUT == "y" || $INPUT == "Y" ]]; then
		setup_hostname
	fi

	echo "finished!"
}

uninstall() {
	echo "uninstalling";

	stop_service gopro
	stop_service led

	systemctl disable gopro.service
	systemctl disable led.service

	rm -f /usr/bin/gopro /usr/bin/led /lib/systemd/system/gopro.service /lib/systemd/system/led.service

	systemctl daemon-reload

	read -p "Remove static ip address configuration? [y|N]: " -r INPUT
	if [[ $INPUT == "y" || $INPUT == "Y" ]]; then
		remove_static_ip
		REBOOT=true
	fi

	read -p "Set hostname (back) to 'raspberrypi'? [y|N]: " -r INPUT
	if [[ $INPUT == "y" || $INPUT == "Y" ]]; then
		setup_hostname "raspberrypi"
	fi

	echo "finished!"
}

help() {
	echo "Install script for the GoPro-Recorder"
	echo -e "\t install \t installs everything needed to for running the GoPro-Recorder permanently"
	echo -e "\t uninstall \t uninstalls everything"
	echo -e "\t check \t\t checks the required dependencies"
	echo -e "\t ip \t\t setups the static ip configuration only"
	echo -e "\t help \t\t shows this help"
}

############################################################################################
# MAIN
############################################################################################

case "$1" in
  ""|''|install)
    install
    ;;
  uninstall)
    uninstall
    ;;
  check)
	check_dependencies
    if [[ $? == 0 ]]; then
    	echo "Success, ready to install!"
    fi
    ;;
  ip)
	echo "Setup static ip."
    setup_static_ip
    ;;
  help)
	help
    ;;
  *)
    echo -e "$0 (install|uninstall|check|ip|help)\n"
    help
    ;;
esac

############################################################################################
# POST-PROCESSING
############################################################################################

# check if we need to reboot
if [ "$REBOOT" == true ] ; then
	read -p "A reboot is required in order to complete installation. Reboot? [y|N]: " -r INPUT
	if [[ $INPUT == "y" || $INPUT == "Y" ]]; then
		reboot
	fi
fi
