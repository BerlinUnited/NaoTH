#!/bin/bash

if [ "$EXTERN_DIR" == "" ]; then
  echo ".::ERROR::. enviroment variable EXTERN_DIR was not set"
  echo ".::ERROR::. will exit"
  exit -1
fi

tar xvzf ../downloads/protobuf-2.3.0.tar.gz
cd protobuf-2.3.0
./configure --prefix=$EXTERN_DIR && make && make install
cd ..
