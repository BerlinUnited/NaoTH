#!/bin/sh
naonr=$1
if [ -z $naonr ]; then
echo "Nao-Nr:"
read naonr
fi

cd $builddir
if [ -f /proc/cpuinfo ]
then
    numcores=`cat /proc/cpuinfo | grep processor | wc -l`
else 
    numcores=`sysctl -n hw.ncpu`
fi

BUILD_PATH=$2
if [ -z $BUILD_PATH ]; then
BUILD_PATH=${NAOTH_BZR}/NaoTHSoccer/Make
fi

DIST_MOTION=${NAOTH_BZR}/NaoTHSoccer/dist/Nao/libnaoth.so
DIST_COGNITION=${NAOTH_BZR}/NaoTHSoccer/dist/Nao/naoth

cd ${BUILD_PATH}
echo "Compile..." && \
premake4 --platform="Nao" gmake && make -R config=optdebugnao  -j $numcores && \
echo "Copy libnaoth.so to 192.168.13.$naonr" && \
(scp $DIST_MOTION nao@192.168.13.$naonr:/home/nao/naoqi/lib/naoqi/ && scp $DIST_COGNITION nao@192.168.13.$naonr:/home/nao/bin/ ) \
|| (echo "Error occurred! [Press Enter to close terminal]"; exit -1) 
