#!/bin/bash

mkdir -p downloads
cd downloads

wget http://ftp.gnome.org/pub/gnome/sources/glib/2.26/glib-2.26.0.tar.gz
wget http://sourceforge.net/projects/sexpr/files/sexpr/1.2.1/sexpr_1.2.1.tar.gz
wget http://protobuf.googlecode.com/files/protobuf-2.3.0.tar.gz

# add more source files to download above

cd ..
