#!/bin/bash
echo "create QT-Creator 2.7+ project files for NaoTHSoccer"

rm -f  ../build/*.config ../build/*.includes ../build/*.creator ../build/*.user ../build/*.shared ../build/*.files ../build/*.make

premake5 qtc27