#!/bin/bash

echo "###### MAKE FILES ######" && \
premake5 --platform=Nao gmake && \
echo "###### C++ ######" && \
cd ../build/ &&
make config=optdebug_nao NaoRobot $@ && \
cd ../Make/ && \
echo "###### XABSL ######" && \
./compileBehavior.sh && \
echo "###### FINISH ######" && exit 0 \

echo "!!!! FAILURE !!!!"
exit 100
