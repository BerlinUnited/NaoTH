#!/bin/bash
echo "create QT-Creator project files for SoccerNaoTH"
premake4 qtc
premake4 --platform="Nao" qtc
