echo "remove old database"
del *.sdf
echo "create VS 2010 project files for SoccerNaoTH"
premake4 vs2010
pause