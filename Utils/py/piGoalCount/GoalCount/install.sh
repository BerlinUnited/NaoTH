#!/bin/bash

HOME_PATH="/home/pi/GoalCount/"

# copy the executable script
chmod +x $HOME_PATH/goalcount
cp $HOME_PATH/goalcount /usr/bin/

#make main python file executable
chmod +x $HOME_PATH/main.py

# install the goalcount.service
#copy the "goalcount.service" file to "/lib/systemd/system/"
cp $HOME_PATH/goalcount.service /lib/systemd/system/
#make it executable
chmod 644 /lib/systemd/system/goalcount.service
#update services
systemctl daemon-reload
systemctl enable goalcount.service

