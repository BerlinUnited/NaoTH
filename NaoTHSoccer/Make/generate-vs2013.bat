echo "remove old database"
del "../build/*.sdf"
echo "create VS 2013 project files for SoccerNaoTH"
premake5 --Test vs2013
pause