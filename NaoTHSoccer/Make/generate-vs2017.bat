echo "remove old database"
del "../build/*.sdf"
echo "create VS 2017 project files for SoccerNaoTH"
premake5 --Test vs2019
pause