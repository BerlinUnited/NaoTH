@echo off
echo "create VS 2008 project files for NaoTHSoccer"
cd NaoTHSoccer\Make
premake4 vs2008
cd ..\..

echo "create VS 2008 project files for ExampleSoccerAgent"
cd ExampleSoccerAgent\Make
premake4 vs2008
cd ..\..

echo "create VS 2008 project files for SimpleSoccerAgent"
cd SimpleSoccerAgent\Make
premake4 vs2008
cd ..\..

echo "create VS 2008 project files for TestArchitecture"
cd TestArchitecture\Make
premake4 vs2008
cd ..\..
