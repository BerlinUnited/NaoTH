#!/bin/bash

if [ "$EXTERN_DIR" == "" ]; then
  echo ".::ERROR::. enviroment variable EXTERN_DIR was not set"
  echo ".::ERROR::. will exit"
  exit -1
fi

if [ "$1" = "check" ]; then
  if [ -d "/usr/include/gtest" -o -d "$EXTERN_DIR/include/gtest" ]; then
    echo "n"
    exit 0
  else
    echo "y" 
    exit 1
  fi
elif [ "$1" = "install" ]; then
  rm -Rf gtest-1.5.0
  unzip ../downloads/gtest-1.5.0.zip
  cd gtest-1.5.0/
  ./configure --prefix=$EXTERN_DIR && make && make install
  cd ..  
fi

