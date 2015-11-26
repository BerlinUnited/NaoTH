#!/bin/sh

#check if the ruby is awaliable and has the right version
ruby="ruby"
s1=$($ruby --version)
s2="$ruby 1.8.9"
if [[ "${s1#*$s2}" == "$s1" ]]
then
  ruby="java -jar $EXTERN_PATH_NATIVE/xabsl-compiler/jruby-complete-1.4.0.jar" 
  echo "compile with jruby: $ruby"
else
  echo "compile with the native ruby version"
fi

$ruby $EXTERN_PATH_NATIVE/xabsl-compiler/xabsl.rb ../Source/Cognition/Modules/Behavior/XABSLBehaviorControl/agents.xabsl -i ../Config/behavior-ic.dat