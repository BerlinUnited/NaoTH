
echo "remove old database"
del "../build/vs2013/*.sdf"

echo "create VS 2013 project files for SoccerNaoTH"
premake5 --Test vs2013

pause