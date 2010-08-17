#!/bin/bash

wget http://sourceforge.net/projects/sexpr/files/sexpr/1.2.1/sexpr_1.2.1.tar.gz/download
tar xvzf sexpr_1.2.1.tar.gz
cd sexpr_1.2.1/
./configure && make
cp src/*.h $EXTERN_DIR/include/
cp src/libsexp.a $EXTERN_DIR/lib/libsfsexp.a
cd ..



