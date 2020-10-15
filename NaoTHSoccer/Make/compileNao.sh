#!/bin/bash

# Any subsequent commands which fail will cause 
# the shell script to exit immediately
set -e

# catch the EXIT and print a message if the exit code is not zero
trap '[ $? -eq 0 ] || echo "!!!! FAILURE WITH CODE $? !!!!"' EXIT

# first parameter should be a build target,
# compile NaoRobot if no argument is provided
# or if the first argument is an optional argument
if [ "${1::1}" == "-" ] || [ "${#}" -eq 0 ]; then
  ACTION=NaoRobot
fi

echo "###### GENERATE MAKE FILES ######"
premake5 --platform=Nao --Test gmake2

echo "###### COMPILE ######"
# run make in the corresponding build dir
(cd ../build/Nao/ && make config=optdebug_nao $ACTION $@)

echo "###### FINISH ######"
