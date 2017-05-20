#check if root first
if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root"
   exit 1
fi

# nao stop
/etc/init.d/naoqi stop

sleep 2

# Set autoload.ini in /etc/naoqi to Naoqi
cat > /home/nao/naoqi/preferences/autoload.ini << EOL
# Here is the minimal list of modules loaded in NaoQi core process required by naoth. 

[core]
albase

[extra]
dcm_hal
alsystem
EOL

# Set autoload.ini in home to Naoqi
cat > /home/nao/naoqi/preferences/autoload.ini << EOL
# autoload.ini
#
# Use this file to list the cross-compiled modules that you wish to load.
# You must specify the full path to the module, python module or program.

[user]
#the/full/path/to/your/liblibraryname.so  # load liblibraryname.so

[python]
#the/full/path/to/your/python_module.py   # load python_module.py

[program]
#the/full/path/to/your/program            # load program
EOL

sleep 2

/etc/init.d/naoqi start
naoth start

