#!/bin/bash

# Any subsequent commands which fail will cause 
# the shell script to exit immediately
set -e

# catch the EXIT and print a message if the exit code is not zero
trap '[ $? -eq 0 ] || echo "!!!! FAILURE WITH CODE $? !!!!"' EXIT

echo "###### GENERATE MAKE FILES ######"
premake5 gmake2

echo "###### COMPILE ######"
# run make in the corresponding build dir
(cd ../build/gmake2/ && make config=optdebug_native ScriptableSimulator $@)

echo "###### FINISH ######"