echo "###### MAKE FILES ######" && \
premake5 --platform=Nao gmake2 && \
echo "###### C++ ######" && \
cd ../build/ &&
make config=optdebug_nao LolaAdaptor $@ && \
cd ../Make/ && \
echo "###### FINISH ######" && exit 0 \

echo "!!!! FAILURE !!!!"
exit 100