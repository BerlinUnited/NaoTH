#!/bin/bash
echo "create QT-Creator project files for SoccerNaoTH"


cd ../../Framework/NaoTH-Commons/Make
rm -f  *.config *.include *.creator *.user *.files *.make

cd ../../Platforms/Make
rm -f  *.config *.include *.creator *.user *.files *.make

cd ../../../NaoTHSoccer/Make
rm -f  *.config *.include *.creator *.user *.files *.make

./generateMakefile.sh
premake4 qtc
premake4 --platform="Nao" qtc
