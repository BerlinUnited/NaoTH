#!/bin/bash

# stop at the first error
set -e

if [ "$EXTERN_DIR" == "" ]; then
  echo ".::ERROR::. enviroment variable EXTERN_DIR was not set"
  echo ".::ERROR::. will exit"
  exit -1
fi

if [ "$1" = "check" ]; then
  if [ -d "/usr/include/gmock" -o -d "$EXTERN_DIR/include/gmock" ]; then
    echo "n"
    exit 0
  else
    echo "y" 
    exit 1
  fi
elif [ "$1" = "install" ]; then
  rm -Rf gmock-1.5.0
  unzip ../downloads/gmock-1.5.0.zip
  cd gmock-1.5.0/
  ./configure --prefix="$EXTERN_DIR" && make -j4 && make install
  cd ..  
fi

