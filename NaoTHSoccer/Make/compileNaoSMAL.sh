#!/bin/bash

# Any subsequent commands which fail will cause 
# the shell script to exit immediately
set -e

# catch the EXIT and print a message if the exit code is not zero
trap '[ $? -eq 0 ] || echo "!!!! FAILURE WITH CODE $? !!!!"' EXIT

echo "###### MAKE FILES ######"
premake5 --platform=Nao gmake2

echo "###### C++ ######"
# run make in the corresponding build dir
(cd ../build/Nao/ && make config=optdebug_nao NaoSMAL $@)

echo "###### FINISH ######"