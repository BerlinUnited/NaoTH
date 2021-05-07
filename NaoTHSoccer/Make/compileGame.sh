#!/bin/bash

# Any subsequent commands which fail will cause 
# the shell script to exit immediately
set -e

# catch the EXIT and print a message if the exit code is not zero
trap '[ $? -eq 0 ] || echo "!!!! FAILURE WITH CODE $? !!!!"' EXIT

echo "###### COMPILE ######"
./compileNao.sh NaoRobot $@

echo "###### XABSL ######"
./compileBehavior.sh

echo "###### FINISH ######"
