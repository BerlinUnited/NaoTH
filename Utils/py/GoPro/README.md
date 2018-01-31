INSTALLATION
==============

Startup-Script:
- copy the "gopro" file to "/etc/init.d"
	sudo cp gopro /etc/init.d/
- make it executable
	sudo chmod 755 /etc/init.d/gopro
- update init scripts
	sudo update-rc.d

GoPro-Controller-Script:
- copy all python files to "/home/pi/GoPro/"
	-> if an other directory is used, the path to the main script in the "gopro" startup script has to be adjusted!
- make main python file executable
	chmod +x /home/pi/GoPro/main.py

Network:
- set a appropiate network ip for the lan interface (eg. eth0); if dhcp isn't available
	-> in order to get the gamecontroller data, the raspi has to be in the "10.0.x.x/16" subnet
	-> make/be sure, there's no other device with the same ip!

Config:
- adjust the "config.py" file for the used GoPro (ssid and password)

RUNNING
=========

If the startup script and the config was set correctly the script starts on the next (re-)boot.

To manually start the gopro-controller with the init-script:
	sudo /etc/init.d/gopro start
	sudo /etc/init.d/gopro stop
	sudo /etc/init.d/gopro status

OR

start the gopro-controller directly
	sudo /home/pi/GoPro/main.py -b -c -v --syslog

For help use:
	/home/pi/GoPro/main.py -h

With other options, one can test, if the raspi can connect to a (new) GoPro
	sudo /home/pi/GoPro/main.py -c -v --syslog
or without config file
	sudo /home/pi/GoPro/main.py -v --syslog -s new_gopro_ssid -p new_gopro_passwd

Logging
=========

By default the gopro-controller just prints outs its log ouput. With the "--syslog" option, the log is written to the
systems log deamon too. To view all output use:
	cat /var/log/syslog

If the gopro-controller is running in the background (using option "-b"), nothing is printed out or logged. Not until
the "--syslog" option is used!
