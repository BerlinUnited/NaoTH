#!/bin/bash
echo "create QT-Creator 2.7+ project files for SoccerNaoTH"

rm -f  ../build/*.config ../build/*.includes ../build/*.creator ../build/*.user ../build/*.files ../build/*.make

./generateMakefile.sh
premake4 qtc27
premake4 --platform="Nao" qtc27
