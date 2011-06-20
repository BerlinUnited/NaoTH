#!/bin/bash

verify_aldir () 
{
  DEFAULT_AL_DIR="/opt/aldebaran/info"
  if  [ "$AL_DIR" == "" ]
  then
        echo "  * AL_DIR has not been set, using default path '$DEFAULT_AL_DIR'"
        export AL_DIR=$DEFAULT_AL_DIR
  fi
  if  [ ! -f $AL_DIR/naoqi/bin/naoqi-bin ]
  then
        echo "WARN: I was unable to find naoqi/bin/naoqi-bin in AL_DIR - the path is probably invalid" 
  fi 
}

verify_alcross () 
{
  DEFAULT_AL_CROSS="$HOME/Roboter/Aldebaran/crosscompiletoolchain"
  if  [ "$AL_CROSS" == "" ]
  then
        echo "  * AL_CROSS has not been set, using default path '$DEFAULT_AL_CROSS'"
        export AL_CROSS=$DEFAULT_AL_CROSS
  fi
  if  [ ! -d $AL_CROSS/staging/usr/lib ]
  then
        echo "WARN: I was unable to find staging/usr/lib in AL_CROSS - the path is probably invalid" 
  fi
}
verify_naothsvn() 
{
  DEFAULT_NAOTH_BZR="$HOME/workspace/Projekte/NaoTH-2011"
  if  [ "$NAOTH_BZR" == "" ]
  then
        echo "  * NAOTH_BZR has not been set, using default path '$DEFAULT_NAOTH_BZR'"
        export NAOTH_BZR=$DEFAULT_NAOTH_BZR
  fi
  if  [ ! -d $NAOTH_BZR/NaoTHSoccer/ ]
  then
        echo "WARN: I was unable to find NaoTHSoccer/ in NAOTH_BZR - the path is probably invalid" 
  fi
}
verify_webotshome () 
{
  DEFAULT_WEBOTS_HOME="/usr/local/webots"
  if  [ "$WEBOTS_HOME" == "" ]
  then
        echo "  * WEBOTS_HOME has not been set, using default path '$DEFAULT_WEBOTS_HOME'"
        export WEBOTS_HOME=$DEFAULT_WEBOTS_HOME
  fi
  if  [ ! -f $WEBOTS_HOME/webots ]
  then
        echo "WARN: I was unable to find webots in WEBOTS_HOME - the path is probably invalid" 
  fi

}
verify_naoimagedir () 
{
  DEFAULT_NAO_IMAGE_DIR="$HOME/Roboter/Aldebaran/images"
  if  [ "$NAO_IMAGE_DIR" == "" ]
  then
        echo "  * NAO_IMAGE_DIR has not been set, using default path '$DEFAULT_NAO_IMAGE_DIR'"
        export NAO_IMAGE_DIR=$DEFAULT_NAO_IMAGE_DIR
  fi
  if  [ ! -d $NAO_IMAGE_DIR ]
  then
        echo "WARN: NAO_IMAGE_DIR does not exist"
  fi
}

echo "Exporting paths"
verify_aldir
verify_alcross
verify_naothsvn
verify_webotshome
verify_naoimagedir
echo "   AL_DIR is $AL_DIR"
echo "   AL_CROSS is $AL_CROSS"
echo "   NAOTH_BZR is $NAOTH_BZR"
echo "   WEBOTS_HOME is $WEBOTS_HOME"
echo "   NAO_IMAGE_DIR is $NAO_IMAGE_DIR"


