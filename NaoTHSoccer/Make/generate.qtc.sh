#!/bin/bash
echo "create QT-Creator 2.7+ project files for NaoTHSoccer"

# remove the old directory
rm -f ../build/qtc27

premake5 --Test qtc27