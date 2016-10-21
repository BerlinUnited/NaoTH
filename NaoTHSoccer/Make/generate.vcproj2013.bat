echo "remove old database"
del "../build/*.sdf"
echo "create VS 2010 project files for SoccerNaoTH"
premake4 vs2013
pause