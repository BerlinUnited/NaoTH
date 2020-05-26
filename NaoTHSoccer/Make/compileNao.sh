#!/bin/bash

# use the first parameter as the buld target, 
# compile NaoRobot if no argument is provided
ACTION=${1:-NaoRobot}

echo "###### MAKE FILES ######" && \
premake5 --platform=Nao --Test gmake2 && \
echo "###### C++ ######" && \
cd ../build/ &&
make config=optdebug_nao $ACTION $@ && \
cd ../Make/ && \
echo "###### FINISH ######" && exit 0 \

echo "!!!! FAILURE !!!!"
exit 100