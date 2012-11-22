#!/bin/bash

if [ "$EXTERN_DIR" == "" ]; then
  echo ".::ERROR::. enviroment variable EXTERN_DIR was not set"
  echo ".::ERROR::. will exit"
  exit -1
fi


if [ "$1" = "check" ]; then
  if [ -d OpenCV-2.3.1 ]; then
    echo "n"
    exit 0
  else
    echo "y" 
    exit 1
  fi
elif [ "$1" = "install" ]; then
  rm -Rf OpenCV-2.3.1
  tar xvjf ../downloads/OpenCV-2.3.1a.tar.bz2
  cd OpenCV-2.3.1/
  mkdir build && cd build
  cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=$EXTERN_DIR ..
  make && make install
  cd ..  
fi

