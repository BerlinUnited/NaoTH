REM add the bin path (premake4 and all the dll)
setx PATH "%path%;%~dp0toolchain_native\extern\bin;"

REM set the path to the nao cross compile toolchain
setx NAO_CTC "%~dp0toolchain_nao"
REM set the path to the external dependencies
setx EXTERN_PATH_NATIVE "%~dp0toolchain_native\extern"


REM add the mintty to the context menu
add_mintty.reg

pause
