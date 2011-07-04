#!/bin/bash

if [ "$EXTERN_DIR" == "" ]; then
  echo ".::ERROR::. enviroment variable EXTERN_DIR was not set"
  echo ".::ERROR::. will exit"
  exit -1
fi

if [ ! -d protobuf-2.4.1 ]; then

  tar xvzf ../downloads/protobuf-2.4.1.tar.gz
  cd protobuf-2.4.1
  ./configure --disable-shared --prefix=$EXTERN_DIR && make && make install
  cd ..
fi
  #workaround for premake4 to be able to forcibly link static
cp $EXTERN_DIR/lib/libprotobuf.a $EXTERN_DIR/lib/libprotobuf-static.a
