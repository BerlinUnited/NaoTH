#!/bin/bash

export EXTERN_DIR="$PWD"

if [ ! -d downloads ]; then
  echo ".::ERROR::. no downloaded files, please execute ./download.sh or download the necessary files by hand"
  echo ".::ERROR::. will exit"
  exit -1
fi

# create _tmp and step into it
mkdir _tmp
cd _tmp

echo "Installing sfsexp library"
. ../install_scripts/sfsexp.sh

echo "Installing glib library"
. ../install_scripts/glib.sh

echo "Installing protobuf"
. ../install_scripts/protobuf.sh

echo "Installing Google Test"
. ../install_scripts/gtest.sh

echo "Installing Google Mock"
. ../install_scripts/gmock.sh

# get out of the _tmp directory and delete it
cd ..
rm -Rf _tmp 

# compile and install projects that are included as source code
cd Make && premake4 gmake && make && premake4 install && cd ..
