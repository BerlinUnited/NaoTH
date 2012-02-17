#!/bin/bash

# LAN
if [ ! -f /etc/init.d/net.eth0 ]
then
    echo "adding rc link for ethernet"
    ln -s /etc/init.d/net.lo /etc/init.d/net.eth0
else
    echo "ethernet link exists"
fi

# WLAN
if [ ! -f /etc/init.d/net.wlan0 ]
then
    echo "adding rc link for wireless"
    ln -s /etc/init.d/net.lo /etc/init.d/net.wlan0
else
    echo "wireless link exists"
fi

# WLAN Encryption
if [ -f ./etc/wpa_supplicant/wpa_supplicant.conf ]
then
    echo "writing wpa_supplicant.conf"
    cp -f ./etc/wpa_supplicant/wpa_supplicant.conf /etc/wpa_supplicant
    chown root:root /etc/wpa_supplicant/wpa_supplicant.conf
    chmod 644 /etc/wpa_supplicant/wpa_supplicant.conf
else
    echo "./etc/wpa_supplicant/wpa_supplicant.conf is missing"
fi

# Network IP Adresses
if [ -f ./etc/conf.d/net ]
then
    echo "writing /etc/conf.d/net"
    cp ./etc/conf.d/net /etc/conf.d/net
    chown root:root /etc/conf.d/net
    chmod 644 /etc/conf.d/net
else
    echo "./etc/conf.d/net is missing"
fi

# Hostname
if [ -f ./etc/hostname ]
then
    echo "writing /etc/hostname"
    cp ./etc/hostname /etc/hostname
    chown root:root /etc/hostname
    chmod 644 /etc/hostname
else
    echo "./etc/conf.d/net is missing"
fi
if [ -f ./etc/conf.d/hostname ]
then
    echo "writing /etc/conf.d/hostname"
    cp ./etc/conf.d/hostname /etc/conf.d/hostname
    chown root:root /etc/conf.d/hostname
    chmod 644 /etc/conf.d/hostname
else
    echo "./etc/conf.d/hostname is missing"
fi

#SSH Config
if [ -f ./etc/ssh.conf/sshd_config ]
then
    echo "writing /etc/conf.d/hostname"
    cp ./etc/ssh.conf/sshd_config /etc/ssh.conf/sshd_config
    chown root:root /etc/ssh.conf/sshd_config
    chmod 644 /etc/ssh.conf/sshd_config
else
    echo "./etc/conf.d/hostname is missing"
fi




# Check and Update Runlevel Configuration for Network Services
chown root:root ./checkRC.sh
chmod 744 ./checkRC.sh
./checkRC.sh "connman=disable net.eth0=boot net.wlan0=boot"
# net.wlan0=boot"






