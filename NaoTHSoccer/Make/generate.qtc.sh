#!/bin/bash
echo "create QT-Creator project files for SoccerNaoTH"

rm -f  ../build/*.config ../build/*.includes ../build/*.creator ../build/*.user ../build/*.files ../build/*.make

./generateMakefile.sh
premake4 qtc
premake4 --platform="Nao" qtc
