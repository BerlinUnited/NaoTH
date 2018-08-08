This setup works with any Raspberry pi with Wi-fi and the following GoPro models: Session 4, GoPro5. It might work with GoPro 3 and 6 as well.

#### INSTALLATION

- connect to the Raspberry Pi
- `cd` to the GoPro directory
- execute the install script `./install.sh`

##### Manual

Setup Startup-Script:
- copy the "gopro" file to "/etc/init.d"  
  `sudo cp gopro /etc/init.d/`
- make it executable  
  `sudo chmod 755 /etc/init.d/gopro`  
- update init scripts  
  `sudo update-rc.d`

Setup GoPro-Controller-Script:
- copy all python files to "/home/pi/GoPro/"

**Note:** if an other directory is used, the path to the main script in the GoPro startup script has to be adjusted!

- make main python file executable
  `chmod +x /home/pi/GoPro/main.py`

Setup Network:
- set a appropiate network ip for the lan interface (eg. eth0); if DHCP isn't available
- set the hostname to a (unique) name; evtl. the logger uses this to write its log files

**Note:** In order to get the gamecontroller data, the Raspberry Pi has to be in the "10.0.x.x/16" subnet. Also make sure, there's no other device with the same IP!  

Config:
- Adjust the "config.py" file for the used GoPro (ssid and password). The information should be provided by the GoPro.

#### RUNNING

If the startup script and the config was set correctly the  init script starts on the next (re-)boot of the Raspberry Pi.

The GoPro Controller know the following commands:  
  `sudo /etc/init.d/gopro start`  
  `sudo /etc/init.d/gopro stop`  
  `sudo /etc/init.d/gopro status` 

To manually start the gopro-controller with the init-script:  
  `sudo /etc/init.d/gopro start`  

OR  

start the gopro-controller directly  
  `sudo /home/pi/GoPro/main.py -b -c -v --syslog`  

For help use:  
  `/home/pi/GoPro/main.py -h`  

With other options, one can test, if the raspi can connect to a (new) GoPro  
  `sudo /home/pi/GoPro/main.py -c -v --syslog`  
or without config file  
  `sudo /home/pi/GoPro/main.py -v --syslog -s new_gopro_ssid -p new_gopro_passwd`  

#### LED States
|  blue | green | red   | Description                                           |
|:-------------:|:-----:|:-----:|-------------------------------------------------------|
| blink (short) |       |       | Wifi network is not available/visible                 |
| blink (1/2s)  |       |       | Currently not connected to network / GoPro            |
| blink (~1s))  |       | blink | Connect to GoPro, but no sd card available            |
|      On       |       |       | Connected to GoPro and ready for recording            |
|               | Off   |       | No GameController available                           |
|               | blink |       | A game vs. INVISIBLES                                 |
|               | On    |       | A game with two teams - GameController is 'connected' |
|               |       | blink | Camera is recording                                   |
|               |       | Off   | Camera is NOT recording                               |
|     Off       | Off   | Off   | Raspi is not powered or isn't connected to anything   |

#### Logging

By default the gopro-controller just prints outs its log ouput. With the "--syslog" option, the log is written to the systems log deamon too. To view all output use:  
  `cat /var/log/syslog`  

If the gopro-controller is running in the background (using option "-b"), nothing is printed out or logged. Not until the "--syslog" option is used!  

Moreover, logs of the recorded games are created in the `/home/pi/GoPro/logs/` directory. Each log contains the file name of the recorded video files seperated by ';'.

#### TESTS
Some tests to check the functionality of the GoPro setup
- normal: run a complete game with 2 teams; the camera should record both halfs and a video log should be written
- normal invisible: run a complete game with only one team; no video should be recorded, but an empty video log is still created
- missing GameController: the leds should indicate that the GameController is missing
- missing wifi: the leds should indicate that
     - ... the wifi network is missing
     - ... the raspi can not log in to the network (not implemented!)
- loosing wifi:
    - not in game / recording: the leds should indicate that
    - while recording: the leds should indicate this, and when the network is back, it should reconnect
- raspi loosing power:
    - not in game / recording
        - and comes back when not in game: raspi should reconnect
        - and comes back when in game: raspi should reconnect and start the recording
        - and comes back and no GameController available: the leds should indicate this
    - while recording
        - and comes back while still in game: raspi should reconnect and continue recording
        - and comes back while not any more in game (GC in finish): raspi should reconnect and stop recording
        - and comes back and no GameController available: raspi should reconnect and stop recording
- manual recording: ???
- camera has no card: the leds should indicate that the card is missing
- long run:
        - normal run, but never press "FINISH": after a certain time, the raspi should stop recording
        - normal run, but never press "PLAY": after a certain time, the raspi should stop recording

