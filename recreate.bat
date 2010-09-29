@echo off
echo "create VS 2008 project files for NaoRunner"
cd NaoRunner\Make
premake4 vs2008
cd ..\..

echo "create VS 2008 project files for DebugCommunication"
cd DebugCommunication\Make
premake4 vs2008
cd ..\..

echo "create VS 2008 project files for SimpleSoccerAgent"
cd SimpleSoccerAgent
premake4 vs2008
cd ..

echo "create VS 2008 project files for Testbehavior"
cd Testbehavior
premake4 vs2008
cd ..
