#!/bin/bash

export EXTERN_DIR="$PWD"

# create _tmp and step into it
mkdir _tmp
cd _tmp

echo "Installing sfsexp library"
. ../install_scripts/sfsexp.sh

echo "Installing glib library"
. ../install_scripts/glib.sh

# get out of the _tmp directory and delete it
cd ..
rm -Rf _tmp 

# complie and install source code
cd Make && premake4 gmake && make && premake4 install && cd ..
