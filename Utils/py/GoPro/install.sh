#!/bin/bash

GOPRO_HOME="/home/pi/GoPro/"

# copy the executable script
chmod +x $GOPRO_HOME/gopro
cp $GOPRO_HOME/gopro /usr/bin/

chmod +x $GOPRO_HOME/led
cp $GOPRO_HOME/led /usr/bin/

#make main python file executable
chmod +x $GOPRO_HOME/main.py
chmod +x $GOPRO_HOME/LEDServer.py

# install the gopro.service
#copy the "gopro.service" file to "/lib/systemd/system/"
cp $GOPRO_HOME/gopro.service /lib/systemd/system/
#make it executable
chmod 644 /lib/systemd/system/gopro.service
#update services
systemctl daemon-reload
systemctl enable gopro.service

# install the led.service
#copy the "led.service" file to "/lib/systemd/system/"
cp $GOPRO_HOME/led.service /lib/systemd/system/
#make it executable
chmod 644 /lib/systemd/system/led.service
#update services
systemctl daemon-reload
systemctl enable led.service
