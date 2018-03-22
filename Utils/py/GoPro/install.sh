#!/bin/bash

GOPRO_HOME="/home/pi/GoPro/"

# copy the executable script
chmod +x $GOPRO_HOME/gopro
cp $GOPRO_HOME/gopro /usr/bin/

#make main python file executable
chmod +x $GOPRO_HOME/main.py

#copy the "gopro.service" file to "/lib/systemd/system/"
cp $GOPRO_HOME/gopro.service /lib/systemd/system/
#make it executable
chmod 644 /lib/systemd/system/gopro.service
#update services
systemctl daemon-reload
systemctl enable gopro.service
