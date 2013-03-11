#!/bin/bash

if [ "$EXTERN_DIR" == "" ]; then
  echo ".::ERROR::. enviroment variable EXTERN_DIR was not set"
  echo ".::ERROR::. will exit"
  exit -1
fi

if [ "$1" = "check" ]; then
  # don't check global install since we have to be sure to have the same version of protobuf on
  # our computer as the one on the Nao
  if [ -d "$EXTERN_DIR/include/google/protobuf/" ]; then
    echo "n"
    exit 0
  else
    echo "y" 
    exit 1
  fi
elif [ "$1" = "install" ]; then
  rm -Rf protobuf-2.4.1
  tar xvzf ../downloads/protobuf-2.4.1.tar.gz
  cd protobuf-2.4.1
  ./configure --disable-shared --prefix=$EXTERN_DIR && make && make install
  cd ..  
fi

