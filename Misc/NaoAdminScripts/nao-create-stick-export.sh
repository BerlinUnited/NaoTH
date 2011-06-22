#!/bin/bash

# I'm checking this in, just in case I'm getting ill or something like that.

# Of course you need to change the path here.

# Always start the script with "sudo ./nao-create-stick-export.sh".
# If not exporting here it looks like the variables are not set 
# (even if set in /etc/profile)

source nao-check-and-set-environment.sh
echo $FOOBAR
echo "starting script"
$NAOTH_BZR/Misc/NaoAdminScripts/nao-create-stick.sh
