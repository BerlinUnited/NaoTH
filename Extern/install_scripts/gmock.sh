#!/bin/bash

if [ "$EXTERN_DIR" == "" ]; then
  echo ".::ERROR::. enviroment variable EXTERN_DIR was not set"
  echo ".::ERROR::. will exit"
  exit -1
fi

if [ ! -d gmock-1.5.0]; then

  unzip ../downloads/gmock-1.5.0.zip
  cd gmock-1.5.0/
  ./configure --prefix=$EXTERN_DIR && make && make install
  cd ..
fi
