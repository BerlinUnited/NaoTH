#!/bin/bash

if [ "$EXTERN_DIR" == "" ]; then
  echo ".::ERROR::. enviroment variable EXTERN_DIR was not set"
  echo ".::ERROR::. will exit"
  exit -1
fi

if [ ! -d OpenCV-2.3.1 ]; then
	tar xvjf ../downloads/OpenCV-2.3.1a.tar.bz2
  cd OpenCV-2.3.1/
  mkdir build && cd build
  cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=$EXTERN_DIR ..
  make && make install
  cd ..
fi



