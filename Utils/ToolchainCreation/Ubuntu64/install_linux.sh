#!/bin/bash

export EXTERN_DIR="$PWD"


PACKAGESTOINSTALL=""


ask_install_package()
{
  # test if installed and choose default answer based on this
  DEFAULT=`../install_scripts/$1.sh check`
  if [ -z "$DEFAULT" ]; then DEFAULT="y" ; fi
  DEFSTRING="[Y/n]"
  if [ "$DEFAULT" = "n" ]; then 
    DEFSTRING="[y/N]"
  fi

  echo -n "Do you want to compile and install \"$1\" to extern/ ? $DEFSTRING : "
  read ANSWER

  # set default answer
  if [ -z "$ANSWER" ]; then 
    ANSWER=$DEFAULT
  fi

  if [ "$ANSWER" = "y" -o "$ANSWER" = "Y" ]
  then
    PACKAGESTOINSTALL="$PACKAGESTOINSTALL $1"
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

echo
ask_install_package "sfsexp"
ask_install_package "glib"
ask_install_package "protobuf"
ask_install_package "gtest"
ask_install_package "gmock"
ask_install_package "opencv"

echo "==========================="
echo "Selected packages for install:"
for PKG in $PACKAGESTOINSTALL
do
   echo "* $PKG"
done

echo -n "Proceed? [Y/n]: "
read ANSWER

if [ "$ANSWER" != "n" ]; then
  # actually install the packages
  for PKG in $PACKAGESTOINSTALL
  do
     echo "==========================="  
     echo "Installing \"$PKG\""
     echo "==========================="
      . ../install_scripts/$PKG.sh install
  done
fi


# get out of the extracted directory
cd ..
