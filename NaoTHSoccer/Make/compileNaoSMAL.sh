echo "###### MAKE FILES ######" && \
premake4 --platform=Nao gmake && \
echo "###### C++ ######" && \
cd ../build/ &&
make -R config=optdebugnao NaoSMAL $@ && \
cd ../Make/ && \
echo "###### FINISH ######" && exit 0 \

echo "!!!! FAILURE !!!!"
exit 100