#!/bin/bash

DATEFILE=/home/nao/date.txt

CURRENT=`date +%s`
if [ -f $DATEFILE ]; then
  OLD=`cat ${DATEFILE}`
else
  OLD="0"
fi

echo $CURRENT > $DATEFILE
chown nao:nao $DATEFILE

echo "old: $OLD current: $CURRENT"

if [ ! "$OLD" -lt "$CURRENT" ]; then
 echo "DATE WRONG!!!!!"
 aplay /opt/naoqi/share/naoqi/wav/buggy.wav
 aplay /opt/naoqi/share/naoqi/wav/buggy.wav
 aplay /opt/naoqi/share/naoqi/wav/buggy.wav
else
 echo "Datecheck ok"
fi

