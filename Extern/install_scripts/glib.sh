#!/bin/bash

if [ "$EXTERN_DIR" == "" ]; then
  echo ".::ERROR::. enviroment variable EXTERN_DIR was not set"
  echo ".::ERROR::. will exit"
  exit -1
fi

if [ ! -d glib-2.26.0 ]; then

  tar xvzf ../downloads/glib-2.26.0.tar.gz
  cd glib-2.26.0/
  ./configure --prefix=$EXTERN_DIR && make && make install
  cd ..
fi



