#!/bin/bash

if [ "$EXTERN_DIR" == "" ]; then
  echo ".::ERROR::. enviroment variable EXTERN_DIR was not set"
  echo ".::ERROR::. will exit"
  exit -1
fi

if [ "$1" = "check" ]; then
  if [ -d "/usr/include/glib-2.0/" -o -d "$EXTERN_DIR/include/glib-2.0" ]; then
    echo "n"
    exit 0
  else
    echo "y" 
    exit 1
  fi
elif [ "$1" = "install" ]; then
  rm -Rf glib-2.26.0
  tar xvzf ../downloads/glib-2.26.0.tar.gz
  cd glib-2.26.0/
  ./configure --prefix=$EXTERN_DIR && make && make install
  cd ..  
fi

