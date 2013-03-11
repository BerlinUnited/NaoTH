#!/bin/bash

if [ -z "$DEFAULT" ]; then DEFAULT="y" ; fi
DEFSTRING="[Y/n]"
if [ "$DEFAULT" = "n" ]; then 
  DEFSTRING="[y/N]"
fi

echo -n "Do you want append NaoTH environment variables to ~/.bashrc? $DEFSTRING : "
read ANSWER

# set default answer
if [ -z "$ANSWER" ]; then 
  ANSWER=$DEFAULT
fi

if [ "$ANSWER" = "y" -o "$ANSWER" = "Y" ]
then
  echo "-----------------------"
  echo "- extending ~/.bashrc -"
  echo "------------------------"

  CURR=`pwd`
  echo "export PATH=\${PATH}:$CURR/toolchain_native/extern/bin:$CURR/toolchain_native/extern/lib # NAOTH" >> ~/.bashrc
  echo "export NAO_CTC=$CURR/toolchain_nao/ # NAOTH" >> ~/.bashrc
  echo "export EXTERN_PATH_NATIVE=$CURR/toolchain_native/extern/ # NAOTH" >> ~/.bashrc
fi



echo "-----------------------------------"
echo "- compiling external dependencies -"
echo "-----------------------------------"

cd toolchain_native/extern/
./install_linux.sh
