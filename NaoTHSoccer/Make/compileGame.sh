#!/bin/bash

echo "###### MAKE FILES ######" && \
premake4 --platform=Nao gmake && \
echo "###### C++ ######" && \
cd ../build/ &&
make -R config=optdebugnao NaoRobot && \
cd ../Make/ && \
echo "###### XABSL ######" && \
./compileBehavior.sh && \
echo "###### FINISH ######" && exit 0 \

echo "!!!! FAILURE !!!!"
exit 100
