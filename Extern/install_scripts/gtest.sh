#!/bin/bash

if [ "$EXTERN_DIR" == "" ]; then
  echo ".::ERROR::. enviroment variable EXTERN_DIR was not set"
  echo ".::ERROR::. will exit"
  exit -1
fi

unzip ../downloads/gtest-1.5.0.zip
cd gtest-1.5.0/
./configure --prefix=$EXTERN_DIR && make && make install
cd ..
