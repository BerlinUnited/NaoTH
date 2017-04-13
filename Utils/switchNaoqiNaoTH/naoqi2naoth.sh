#check if root first
if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root"
   exit 1
fi

# nao stop
/etc/init.d/naoqi stop

sleep 2

# Check if files for switching exists
if [ ! -f "/home/nao/naoqi/preferences/autoload.ini.naoth" ]; then
  echo "File /home/nao/naoqi/preferences/autoload.ini.naoth not found."
  exit 1
fi
if [ ! -f "/etc/naoqi/autoload.ini.naoth" ]; then
  echo "File /etc/naoqi/autoload.ini.naoth not found."
  exit 1
fi

# switch autoload in home
# backup current autoload as bak # TODO dont call it bak
mv /home/nao/naoqi/preferences/autoload.ini /home/nao/naoqi/preferences/autoload.ini.bak

# assumes *.naoth is the naoqi autoload
mv /home/nao/naoqi/preferences/autoload.ini.naoth /home/nao/naoqi/preferences/autoload.ini

# switch autoload in /etc/
mv /etc/naoqi/autoload.ini /etc/naoqi/autoload.ini.bak
mv /etc/naoqi/autoload.ini.naoth /etc/naoqi/autoload.ini

sleep 2

/etc/init.d/naoqi start
naoth start
