
echo "remove old database"
del "../build/vs2017/*.sdf"

echo "create VS 2017 project files for SoccerNaoTH"
premake5 --Test vs2017

pause