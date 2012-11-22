#!/bin/bash

export EXTERN_DIR="$PWD"

install_package()
{
  # test if installed and choose default answer based on this
  DEFAULT=`../install_scripts/$1.sh check`
  if [ -z "$DEFAULT" ]; then DEFAULT="y" ; fi
  DEFSTRING="[Y/n]"
  if [ "$DEFAULT" = "n" ]; then 
    DEFSTRING="[y/N]"
  fi

  echo
  echo -n "Do you want to compile and install \"$1\" to Extern/ ? $DEFSTRING : "
  read ANSWER

  # set default answer
  if [ -z "$ANSWER"]; then 
    ANSWER=$DEFAULT
  fi

  if [ "$ANSWER" = "y" -o "$ANSWER" = "Y" ]
  then
    echo "Installing \"$1\""
    . ../install_scripts/$1.sh install
  else
    echo "*NOT* installing \"$1\", install it with your package manager"
  fi
}

if [ ! -d downloads ]; then
  echo ".::ERROR::. no downloaded files, please execute ./download.sh or download the necessary files by hand"
  echo ".::ERROR::. will exit"
  exit -1
fi

# create extracted dir and step into it
mkdir -p extracted
cd extracted

install_package "sfsexp"
install_package "glib"
install_package "protobuf"
install_package "gtest"
install_package "gmock"
install_package "opencv"
#echo "Installing sfsexp library"
#. ../install_scripts/sfsexp.sh
#
#echo "Installing glib library"
#. ../install_scripts/glib.sh
#
#echo "Installing protobuf"
#. ../install_scripts/protobuf.sh
#
#echo "Installing Google Test"
#. ../install_scripts/gtest.sh
#
#echo "Installing Google Mock"
#. ../install_scripts/gmock.sh
#
#echo "Installing OpenCV"
#. ../install_scripts/opencv.sh


# get out of the extracted directory
cd ..

# compile and install projects that are included as source code
# cd Make && premake4 gmake && make && premake4 install && cd ..
