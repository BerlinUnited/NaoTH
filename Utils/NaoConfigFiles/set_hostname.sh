#!/bin/bash
# -------- configure the host name --------

if [ -f /etc/hostname ]
then
    echo $1 > /etc/hostname;
else
    echo "/etc/hostname is missing";
fi

if [ -f /etc/conf.d/hostname ]
then
    echo "hostname=\"$1\"" > /etc/conf.d/hostname;
else
    echo "/etc/conf.d/hostname is missing";
fi

hostname $1



