#!/bin/bash

# Any subsequent commands which fail will cause 
# the shell script to exit immediately
set -e

# catch the EXIT and print a message if the exit code is not zero
trap '[ $? -eq 0 ] || echo "!!!! FAILURE WITH CODE $? !!!!"' EXIT

# use the first parameter as the buld target, 
# compile NaoRobot if no argument is provided
ACTION=${1:-NaoRobot}

echo "###### GENERATE MAKE FILES ######"
premake5 --platform=Nao --Test gmake2

echo "###### COMPILE ######"
# run make in the corresponding build dir
(cd ../build/Nao/ && make config=optdebug_nao $ACTION $@)

echo "###### FINISH ######"