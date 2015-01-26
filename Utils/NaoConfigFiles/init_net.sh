#!/bin/bash

# -------- network dependent --------

# WLAN Encryption
if [ -f ./etc/wpa_supplicant/wpa_supplicant.conf ]
then
    echo "writing wpa_supplicant.conf";
    cp -f ./etc/wpa_supplicant/wpa_supplicant.conf /etc/wpa_supplicant;
    chown root:root /etc/wpa_supplicant/wpa_supplicant.conf;
    chmod 644 /etc/wpa_supplicant/wpa_supplicant.conf;
else
    echo "./etc/wpa_supplicant/wpa_supplicant.conf is missing";
fi

# Network IP Adresses
if [ -f ./etc/conf.d/net ]
then
    echo "writing /etc/conf.d/net";
    cp ./etc/conf.d/net /etc/conf.d/net;
    chown root:root /etc/conf.d/net;
    chmod 644 /etc/conf.d/net;
else
    echo "./etc/conf.d/net is missing";
fi


# restart the network
/etc/init.d/net.eth0 restart
/etc/init.d/net.wlan0 restart





