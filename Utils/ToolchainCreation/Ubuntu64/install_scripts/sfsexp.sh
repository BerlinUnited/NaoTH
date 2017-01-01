#!/bin/bash

# stop at the first error
set -e

if [ "$EXTERN_DIR" == "" ]; then
  echo ".::ERROR::. enviroment variable EXTERN_DIR was not set"
  echo ".::ERROR::. will exit"
  exit -1
fi

if [ "$1" = "check" ]; then
  if [ -f "/usr/include/sexp.h" -o -f "$EXTERN_DIR/include/sexp.h" ]; then
    echo "n"
    exit 0
  else
    echo "y" 
    exit 1
  fi
elif [ "$1" = "install" ]; then
  rm -Rf sexpr_1.2.1
  tar xvzf ../downloads/sexpr_1.2.1.tar.gz
  cd sexpr_1.2.1/
  ./configure && make
  
  # create the dirs if they are not there
  mkdir -p "$EXTERN_DIR/include/sfsexp"
  mkdir -p "$EXTERN_DIR/lib"
  
  # copy the results
  cp src/*.h "$EXTERN_DIR/include/sfsexp"
  cp src/libsexp.a "$EXTERN_DIR/lib/libsfsexp.a"
  cd ..  
fi



