echo "remove old database"
del "../build/*.sdf"
echo "create VS 2017 project files for SoccerNaoTH"
premake5 --JNI --Test vs2017
pause