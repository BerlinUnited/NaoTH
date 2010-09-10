#!/bin/bash

if [ "$EXTERN_DIR" == "" ]; then
  echo ".::ERROR::. enviroment variable EXTERN_DIR was not set"
  echo ".::ERROR::. will exit"
  exit -1
fi

wget http://ftp.gnome.org/pub/gnome/sources/glib/2.24/glib-2.24.2.tar.gz
tar xvzf glib-2.24.2.tar.gz
cd glib-2.24.2/
./configure --prefix=$EXTERN_DIR && make && make install
cd ..



