#!/bin/bash

mkdir -p downloads
cd downloads

wget -N http://ftp.gnome.org/pub/gnome/sources/glib/2.26/glib-2.26.0.tar.gz
wget -N http://sourceforge.net/projects/sexpr/files/sexpr/1.2.1/sexpr_1.2.1.tar.gz
wget -N http://protobuf.googlecode.com/files/protobuf-2.4.1.tar.gz
wget -N http://googletest.googlecode.com/files/gtest-1.5.0.zip
wget -N http://googlemock.googlecode.com/files/gmock-1.5.0.zip
# add more source files to download above

cd ..
