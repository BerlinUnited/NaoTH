#!/bin/bash

rcList=$1
runLevels="boot default disable"

for rc in $rcList
do
    targetRunlevel=${rc/[a-zA-Z]*[=]/};
    targetRC=${rc/[=]*[a-zA-Z]*/};
    targetRCFound=0;
    if [ -f /etc/init.d/$targetRC ]
    then
	for runLevel in $runLevels
	do
	    if [ $runLevel != "disable" ]
	    then
		rcConfigList=`rc-config list $runLevel`;
		for configRC in $rcConfigList
		do
		    if [ $targetRC == $configRC ]
		    then
			targetRCFound=1
			if ( [ $targetRunlevel != $runLevel ] || [ $targetRunlevel == "disable" ] )
			then
			    echo "removing '$targetRC' '$runLevel'";
			    rc-update del $targetRC $runLevel;
			else
			    if [ $targetRunlevel == $runLevel ]
			    then
				echo "nothing to do for '$targetRC' '$runLevel'";
			    fi
			fi
		    fi
		done
	    fi
        done
	if ( [ $targetRCFound == 0 ] && [ $targetRunlevel != "disable" ] )
	then
	    echo "adding '$targetRC' '$targetRunlevel'";
	    rc-update add $targetRC $targetRunlevel;
	fi
    else
	echo "'$targetRC' not available";
    fi
done

