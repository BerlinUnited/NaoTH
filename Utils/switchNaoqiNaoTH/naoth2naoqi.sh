#check if root first
if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root"
   exit 1
fi

naoth stop

# nao stop
/etc/init.d/naoqi stop
sleep 2

# Check if files for switching exists
if [ ! -f "/home/nao/naoqi/preferences/autoload.ini.bak" ]; then
  echo "File /home/nao/naoqi/preferences/autoload.ini.bak not found."
  exit 1
fi
if [ ! -f "/etc/naoqi/autoload.ini.bak" ]; then
  echo "File /etc/naoqi/autoload.ini.bak not found."
  exit 1
fi



# switch autoload in /home/
# backup current autoload as naoth - assumes naoth is set
mv /home/nao/naoqi/preferences/autoload.ini /home/nao/naoqi/preferences/autoload.ini.naoth

# assumes *.bak is the naoqi autoload and exists
mv /home/nao/naoqi/preferences/autoload.ini.bak /home/nao/naoqi/preferences/autoload.ini


# switch autoload in /etc/
# backup current autoload as naoth
mv /etc/naoqi/autoload.ini /etc/naoqi/autoload.ini.naoth
# assumes *.bak is the naoqi autoload
mv /etc/naoqi/autoload.ini.bak /etc/naoqi/autoload.ini

sleep 2
# nao start
if [ -f "./deploy/home/nao/bin/libnaosmal.so" ]; then
  /etc/init.d/naoqi start
fi
